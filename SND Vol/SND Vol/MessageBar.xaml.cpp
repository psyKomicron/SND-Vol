#include "pch.h"
#include "MessageBar.xaml.h"
#if __has_include("MessageBar.g.cpp")
#include "MessageBar.g.cpp"
#endif

#include <regex>

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace winrt::Windows::Foundation;


namespace winrt::SND_Vol::implementation
{
    MessageBar::MessageBar()
    {
        InitializeComponent();

        timer = DispatcherQueue().CreateTimer();
        auto duration = (Application::Current().Resources().Lookup(box_value(L"MessageBarIntervalSeconds")).as<int32_t>() * 1000) + 150;
        timer.Interval(
            std::chrono::milliseconds(duration)
        );
        timer.Tick({ this, &MessageBar::DispatcherQueueTimer_Tick });
    }

    void MessageBar::EnqueueMessage(const IInspectable& message)
    {
        {
            std::unique_lock<std::mutex> lock{ messageQueueMutex };
            messageQueue.push(message);
        }

        if (!timer.IsRunning()) 
        {
            DisplayMessage();
            timer.Start();
        }
    }

    void MessageBar::EnqueueString(const winrt::hstring& message)
    {
        EnqueueMessage(box_value(message));
    }

    void MessageBar::CloseButton_Click(winrt::Windows::Foundation::IInspectable const&, RoutedEventArgs const&)
    {
        timer.Stop();
        TimerProgressBarStoryboard().Stop();
    }

    void MessageBar::UserControl_Loaded(IInspectable const&, RoutedEventArgs const&)
    {
        TimerProgressBar_SizeChanged(nullptr, nullptr);
    }

    void MessageBar::TimerProgressBar_SizeChanged(IInspectable const&, SizeChangedEventArgs const&)
    {
        TimerProgressBarClipping().Rect(Rect(0.f, 0.f, static_cast<float>(BackgroundTimerBorder().ActualWidth()), static_cast<float>(BackgroundTimerBorder().ActualHeight())));
    }


    void MessageBar::DisplayMessage()
    {
        // Dequeue a message, send it to the UI
        IInspectable message = nullptr;
        {
            std::unique_lock<std::mutex> lock{ messageQueueMutex };

            if (!messageQueue.empty())
            {
                message = messageQueue.front();
                messageQueue.pop();
            }
            else
            {
                timer.Stop();
                // Hide the control.
                VisualStateManager::GoToState(*this, L"Collapsed", true);
            }
        }

        if (message)
        {
            if (std::optional<hstring> hs = message.try_as<hstring>())
            {
                static std::wregex word{ L"(\\w+)", std::regex_constants::optimize };
                std::wstring text{ hs.value()};
                auto iterator = std::wsregex_iterator(text.begin(), text.end(), word);
                auto ptrDiff = std::distance(iterator, std::wsregex_iterator());
                if (ptrDiff > 0)
                {
                    int64_t msCount = (ptrDiff / 3.5) * 1000;
                    if (msCount < 1000)
                    {
                        msCount = 1000;
                    }

                    timer.Interval(
                        std::chrono::milliseconds(msCount + 150)
                    );
                    TimerProgressBarAnimation().Duration(
                        DurationHelper::FromTimeSpan(std::chrono::milliseconds(msCount))
                    );
                }
            }

            if (Visibility() == Visibility::Collapsed)
            {
                // Show the control.
                VisualStateManager::GoToState(*this, L"Visible", true);
            }

            MainContentPresenter().Content(box_value(message));
            TimerProgressBarStoryboard().Begin();
        }
    }

    void MessageBar::DispatcherQueueTimer_Tick(winrt::Microsoft::UI::Dispatching::DispatcherQueueTimer, winrt::Windows::Foundation::IInspectable)
    {
        DisplayMessage();
    }
}
