#pragma once
#include <JuceHeader.h>

class CategoryAssignComponent : public juce::Component
{
public:
    CategoryAssignComponent(const juce::StringArray& categories, std::function<void(const juce::String&)> onAssign)
    {
        addAndMakeVisible(categoryBox);
    categoryBox.addItem("None", 1);
    for (const auto &cat : categories)
      categoryBox.addItem(cat, categoryBox.getNumItems() + 1);
        categoryBox.setSelectedId(1);

        addAndMakeVisible(assignButton);
        assignButton.setButtonText("Assign");

        assignButton.onClick = [this, onAssign] {
          if (categoryBox.getText() != "None") {
            if (onAssign)
              onAssign(categoryBox.getText());
          }
          if (auto *w = findParentComponentOfClass<juce::DialogWindow>())
            w->closeButtonPressed();
        };

        addAndMakeVisible(cancelButton);
        cancelButton.setButtonText("Cancel");
        cancelButton.onClick = [this] {
            if (auto* w = findParentComponentOfClass<juce::DialogWindow>())
                w->closeButtonPressed();
        };

        setSize(320, 180);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(20);
        categoryBox.setBounds(area.removeFromTop(50));
        area.removeFromTop(20);
        assignButton.setBounds(area.removeFromLeft(area.getWidth() / 2).reduced(5));
        cancelButton.setBounds(area.reduced(5));
    }

    void paint(juce::Graphics& g) override
    {
    }

private:
    juce::ComboBox categoryBox;
    juce::TextButton assignButton, cancelButton;
};
