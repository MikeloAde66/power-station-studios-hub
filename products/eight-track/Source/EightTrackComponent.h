// EightTrackComponent.h
// Power Station Studios — 8-Track module
//
// Full chassis redesign (v2) matching the reference mockup's actual
// layout: navy header strip ("8-Track" + Options), cream status strip
// (status message + Settings), a row of 4 direct program-select
// buttons, a housed meter panel (indicator light + twin VU meters) on
// a wood body, and the gold transport button row along the bottom.
// General vintage-8-track shape and function — no reproduction of any
// specific branded unit's logo/livery.
//
// Known gaps:
//   - VU needles are cosmetic only — driven by a random jitter while
//     "playing", not by actual signal level. Real level metering would
//     need peak/RMS measurement wired in from processBlock().
//   - Power/Auto/Fast Fwd/Pause/Options buttons are drawn and clickable
//     but not wired to engine behavior beyond Power (toggles isPlaying)
//     and the 4 program-select buttons (call onProgramSelected, wired
//     to the real EightTrackCartridgeEngine::setProgram in the
//     processor). Options/Settings have callback hooks but no actual
//     settings panel built yet.
//   - Status strip is empty by default and only shows text via
//     setStatusMessage(), which nothing calls yet. It must stay silent
//     until a real state change warrants it -- it previously defaulted
//     to a hardcoded "audio input is muted" warning shown on open
//     before the user had done anything, which was wrong and has been
//     removed.

#pragma once
#include <JuceHeader.h>

class EightTrackComponent : public juce::Component, public juce::Timer
{
public:
    EightTrackComponent()
    {
        // --- Header ---
        addAndMakeVisible (optionsButton);
        optionsButton.onClick = [this] { if (onOptionsClicked) onOptionsClicked(); };

        // --- Status strip ---
        addAndMakeVisible (settingsButton);
        settingsButton.onClick = [this] { if (onSettingsClicked) onSettingsClicked(); };

        // --- Program select row (direct 1-4 selection) ---
        for (int i = 0; i < 4; ++i)
        {
            auto& b = programButtons[i];
            b.setButtonText (juce::String (i + 1));
            b.setClickingTogglesState (false);
            addAndMakeVisible (b);
            b.onClick = [this, program = i + 1]
            {
                setCurrentProgram (program);
                if (onProgramSelected)
                    onProgramSelected (program);
            };
        }
        updateProgramButtonColours();

        // --- Bottom transport row ---
        for (auto* b : { &powerButton, &autoButton, &ejectButton, &fastFwdButton, &pauseButton })
            addAndMakeVisible (*b);

        addAndMakeVisible (programButton);
        programButton.onClick = [this] { if (onSwitchProgram) onSwitchProgram(); };

        powerButton.setClickingTogglesState (true);
        powerButton.onClick = [this] { if (onPowerToggled) onPowerToggled (powerButton.getToggleState()); };

        startTimerHz (30);
    }

    std::function<void()> onSwitchProgram;
    std::function<void (int program1to4)> onProgramSelected;
    std::function<void (bool isPowered)> onPowerToggled;
    std::function<void()> onOptionsClicked;
    std::function<void()> onSettingsClicked;

    void setCurrentProgram (int program1to4)
    {
        currentProgram = juce::jlimit (1, 4, program1to4);
        updateProgramButtonColours();
        repaint();
    }

    void setStatusMessage (const juce::String& message)
    {
        statusMessage = message;
        repaint();
    }

    void setPlaying (bool shouldPlay)
    {
        isPlaying = shouldPlay;
        if (! isPlaying)
        {
            leftVu = 0.0f;
            rightVu = 0.0f;
        }
    }

    void resized() override
    {
        auto bounds = getLocalBounds();

        // Header strip
        headerBounds = bounds.removeFromTop (40).toFloat();
        auto headerInt = headerBounds.toNearestInt();
        optionsButton.setBounds (headerInt.removeFromLeft (92).reduced (6, 7));

        // Status strip
        statusBounds = bounds.removeFromTop (36).toFloat();
        auto statusInt = statusBounds.toNearestInt();
        settingsButton.setBounds (statusInt.removeFromRight (112).reduced (6, 4));

        bounds.removeFromTop (6); // accent-line gap

        // Program select row
        auto programRow = bounds.removeFromTop (44);
        constexpr int btnW = 64, gap = 10;
        auto row = programRow.withSizeKeepingCentre (btnW * 4 + gap * 3, 34);
        for (int i = 0; i < 4; ++i)
        {
            programButtons[i].setBounds (row.removeFromLeft (btnW));
            row.removeFromLeft (gap);
        }

        bounds.removeFromTop (10); // gap before meter panel

        // Bottom transport row (reserved first so meter panel gets the rest)
        auto buttonRow = bounds.removeFromBottom (48);
        int buttonWidth = buttonRow.getWidth() / 6;
        powerButton.setBounds (buttonRow.removeFromLeft (buttonWidth).reduced (4));
        autoButton.setBounds (buttonRow.removeFromLeft (buttonWidth).reduced (4));
        ejectButton.setBounds (buttonRow.removeFromLeft (buttonWidth).reduced (4));
        programButton.setBounds (buttonRow.removeFromLeft (buttonWidth).reduced (4));
        fastFwdButton.setBounds (buttonRow.removeFromLeft (buttonWidth).reduced (4));
        pauseButton.setBounds (buttonRow.removeFromLeft (buttonWidth).reduced (4));

        meterPanelBounds = bounds.reduced (14, 6).toFloat();
    }

    void paint (juce::Graphics& g) override
    {
        auto full = getLocalBounds().toFloat();

        // Wood-tone body (base layer, behind the header/status strips)
        juce::ColourGradient woodGradient (juce::Colour::fromRGB (92, 62, 40), full.getX(), full.getY(),
                                            juce::Colour::fromRGB (62, 40, 26), full.getX(), full.getBottom(), false);
        g.setGradientFill (woodGradient);
        g.fillAll();

        drawHeader (g);
        drawStatusStrip (g);
        drawMeterPanel (g);
    }

private:
    // Sets each program-select button's own colour so the active one
    // reads as "lit" (orange) and the rest stay slate. This has to be
    // done via button.setColour() rather than drawn underneath the
    // button in this component's paint() -- JUCE paints child
    // components (the real TextButtons) on top of the parent's paint(),
    // so anything drawn here for "under" the buttons would never be
    // visible. EightTrackSkinLookAndFeel::drawButtonBackground now
    // honors each button's own colour instead of hardcoding one colour
    // for every button in the theme.
    void updateProgramButtonColours()
    {
        for (int i = 0; i < 4; ++i)
        {
            bool isActive = (i + 1 == currentProgram);
            programButtons[i].setColour (juce::TextButton::buttonColourId,
                                          isActive ? juce::Colour (0xFFD9652B)
                                                   : juce::Colour (0xFF3A3F47));
        }
    }

    void drawHeader (juce::Graphics& g)
    {
        g.setColour (juce::Colour::fromRGB (42, 54, 62)); // navy
        g.fillRect (headerBounds);

        g.setColour (juce::Colours::white);
        g.setFont (juce::Font (22.0f, juce::Font::bold));
        g.drawText ("8-Track", headerBounds, juce::Justification::centred);
    }

    void drawStatusStrip (juce::Graphics& g)
    {
        g.setColour (juce::Colour::fromRGB (245, 239, 201)); // cream
        g.fillRect (statusBounds);

        auto textArea = statusBounds.reduced (16.0f, 0.0f).withTrimmedRight (120.0f);
        g.setColour (juce::Colour::fromRGB (40, 36, 20));
        g.setFont (juce::Font (16.0f));
        g.drawFittedText (statusMessage, textArea.toNearestInt(), juce::Justification::centredLeft, 1);

        // Accent line under the status strip
        g.setColour (juce::Colour::fromRGB (196, 140, 60));
        g.fillRect (statusBounds.getX(), statusBounds.getBottom(), statusBounds.getWidth(), 2.0f);
    }

    void drawMeterPanel (juce::Graphics& g)
    {
        auto panel = meterPanelBounds;

        // Light-gray housing
        g.setColour (juce::Colour::fromRGB (176, 176, 180));
        g.fillRoundedRectangle (panel, 6.0f);
        g.setColour (juce::Colour::fromRGB (120, 120, 124));
        g.drawRoundedRectangle (panel.reduced (1.0f), 6.0f, 1.5f);

        auto inner = panel.reduced (panel.getWidth() * 0.025f, panel.getHeight() * 0.06f);

        // Indicator light (left third)
        auto indicatorArea = inner.removeFromLeft (inner.getWidth() * 0.32f);
        drawIndicatorLight (g, indicatorArea);

        inner.removeFromLeft (inner.getWidth() * 0.04f);

        // Twin VU meters (remaining area, split L/R)
        auto leftMeterArea = inner.removeFromLeft (inner.getWidth() * 0.48f);
        inner.removeFromLeft (inner.getWidth() * 0.08f);
        auto rightMeterArea = inner;

        drawVuMeter (g, leftMeterArea, leftVu, "L");
        drawVuMeter (g, rightMeterArea, rightVu, "R");
    }

    void drawIndicatorLight (juce::Graphics& g, juce::Rectangle<float> area)
    {
        area = area.reduced (area.getWidth() * 0.08f, area.getHeight() * 0.12f);

        g.setColour (juce::Colours::black);
        g.fillRoundedRectangle (area, 3.0f);

        // Glowing red "cartridge loaded" indicator — brighter while playing
        auto light = area.reduced (area.getWidth() * 0.1f, area.getHeight() * 0.18f);
        light = light.withHeight (light.getHeight() * 0.62f);

        float glow = isPlaying ? 1.0f : 0.55f;
        g.setColour (juce::Colour::fromFloatRGBA (0.78f * glow, 0.18f * glow, 0.14f * glow, 1.0f));
        g.fillRoundedRectangle (light, 2.0f);
        g.setColour (juce::Colours::black.withAlpha (0.4f));
        g.drawRoundedRectangle (light, 2.0f, 1.0f);
    }

    void drawVuMeter (juce::Graphics& g, juce::Rectangle<float> area, float level, const juce::String& label)
    {
        area = area.reduced (area.getWidth() * 0.05f);

        g.setColour (juce::Colour::fromRGB (30, 24, 16));
        g.fillRoundedRectangle (area, 3.0f);

        auto center = juce::Point<float> (area.getCentreX(), area.getBottom() - area.getHeight() * 0.12f);
        float needleLength = area.getHeight() * 0.75f;

        // Arc scale
        g.setColour (juce::Colours::goldenrod.withAlpha (0.6f));
        juce::Path arc;
        arc.addArc (center.x - needleLength, center.y - needleLength, needleLength * 2.0f, needleLength * 2.0f,
                    juce::degreesToRadians (200.0f), juce::degreesToRadians (340.0f), true);
        g.strokePath (arc, juce::PathStrokeType (1.5f));

        // Needle: -40 (left, level 0) to +40 degrees (right, level 1) off vertical
        float angle = juce::jmap (level, 0.0f, 1.0f,
                                   juce::degreesToRadians (-50.0f), juce::degreesToRadians (50.0f));
        juce::Point<float> needleTip (center.x + needleLength * std::sin (angle),
                                       center.y - needleLength * std::cos (angle));
        g.setColour (juce::Colours::black);
        g.drawLine (center.x, center.y, needleTip.x, needleTip.y, 2.0f);
        g.fillEllipse (center.x - 3.0f, center.y - 3.0f, 6.0f, 6.0f);

        g.setColour (juce::Colours::whitesmoke);
        g.setFont (juce::Font (juce::jmax (10.0f, area.getHeight() * 0.12f), juce::Font::bold));
        g.drawText (label, area.removeFromTop (area.getHeight() * 0.2f), juce::Justification::centred);
    }

    void timerCallback() override
    {
        if (isPlaying)
        {
            leftVu  = juce::jlimit (0.0f, 1.0f, leftVu  + random.nextFloat() * 0.3f - 0.15f);
            rightVu = juce::jlimit (0.0f, 1.0f, rightVu + random.nextFloat() * 0.3f - 0.15f);
        }
        repaint();
    }

    // Header / status strip
    juce::TextButton optionsButton  { "Options" };
    juce::TextButton settingsButton { "Settings..." };
    juce::Rectangle<float> headerBounds, statusBounds;

    // Empty by default -- this strip must stay silent until something
    // real actually happens. It previously defaulted to a hardcoded
    // "audio input is muted" warning that showed up the instant the
    // plugin opened, before the user had done anything. Only
    // setStatusMessage() should populate this, and only in response to
    // an actual state change.
    juce::String statusMessage;

    // Direct program-select row (text set in the constructor loop)
    juce::TextButton programButtons[4];

    // Transport row
    juce::TextButton powerButton   { "Power" };
    juce::TextButton autoButton    { "Auto" };
    juce::TextButton ejectButton   { "Eject" };
    juce::TextButton programButton { "Program" };
    juce::TextButton fastFwdButton { "Fwd" };
    juce::TextButton pauseButton   { "Pause" };

    // Meter panel
    juce::Rectangle<float> meterPanelBounds;

    int currentProgram = 1;
    bool isPlaying = false;
    float leftVu = 0.0f;
    float rightVu = 0.0f;
    juce::Random random;
};
