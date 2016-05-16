/**********************************************************************

Audacity: A Digital Audio Editor

Scrubbing.h

Paul Licameli split from TrackPanel.cpp

**********************************************************************/

#ifndef __AUDACITY_SCRUBBING__
#define __AUDACITY_SCRUBBING__

#include "../../MemoryX.h"
#include <vector>
#include <wx/event.h>
#include <wx/longlong.h>

#include "../../Experimental.h"
#include "../../widgets/Overlay.h"

class AudacityProject;

// Scrub state object
class Scrubber : public wxEvtHandler
{
public:
   Scrubber(AudacityProject *project);
   ~Scrubber();

   // Assume xx is relative to the left edge of TrackPanel!
   void MarkScrubStart(
      wxCoord xx, bool smoothScrolling,
      bool alwaysSeeking // if false, can switch seeking or scrubbing
                           // by mouse button state
   );

   // Returns true iff the event should be considered consumed by this:
   // Assume xx is relative to the left edge of TrackPanel!
   bool MaybeStartScrubbing(wxCoord xx);

   void ContinueScrubbing();

   // This is meant to be called only from ControlToolBar
   void StopScrubbing();

   wxCoord GetScrubStartPosition() const
   { return mScrubStartPosition; }

   // True iff the user has clicked to start scrub and not yet stopped,
   // but IsScrubbing() may yet be false
   bool HasStartedScrubbing() const
   { return GetScrubStartPosition() >= 0; }
   bool IsScrubbing() const;

   bool IsScrollScrubbing() const // If true, implies HasStartedScrubbing()
   { return mSmoothScrollingScrub; }

   bool IsAlwaysSeeking() const
   { return mAlwaysSeeking; }

   bool ShouldDrawScrubSpeed();
   double FindScrubSpeed(bool seeking, double time) const;
   double GetMaxScrubSpeed() const { return mMaxScrubSpeed; }

   void HandleScrollWheel(int steps);

   bool PollIsSeeking();

   // This returns the same as the enabled state of the menu items:
   bool CanScrub() const;

   // For the toolbar
   void AddMenuItems();
   // For popup
   void PopulateMenu(wxMenu &menu);

   void OnScrub(wxCommandEvent&);
   void OnScrollScrub(wxCommandEvent&);
   void OnSeek(wxCommandEvent&);
   void OnScrollSeek(wxCommandEvent&);

   // A string to put in the leftmost part of the status bar.
   const wxString &GetUntranslatedStateString() const;

   // All possible status strings.
   static std::vector<wxString> GetAllUntranslatedStatusStrings();

   void Pause(bool paused);

private:
   void DoScrub(bool scroll, bool seek);
   void OnActivateOrDeactivateApp(wxActivateEvent & event);
   void UncheckAllMenuItems();
   void CheckMenuItem();

   // I need this because I can't push the scrubber as an event handler
   // in two places at once.
   struct Forwarder : public wxEvtHandler {
      Forwarder(Scrubber &scrubber_) : scrubber( scrubber_ ) {}

      Scrubber &scrubber;

      void OnMouse(wxMouseEvent &event);
      DECLARE_EVENT_TABLE()
   };
   Forwarder mForwarder{ *this };

private:
   int mScrubToken;
   wxLongLong mScrubStartClockTimeMillis;
   bool mScrubHasFocus;
   int mScrubSpeedDisplayCountdown;
   wxCoord mScrubStartPosition;
   wxCoord mLastScrubPosition {};
   double mMaxScrubSpeed;
   bool mScrubSeekPress;
   bool mSmoothScrollingScrub;
   bool mAlwaysSeeking {};
   bool mDragging {};

#ifdef EXPERIMENTAL_SCRUBBING_SCROLL_WHEEL
   int mLogMaxScrubSpeed;
#endif

   AudacityProject *mProject;

   DECLARE_EVENT_TABLE()

   class ScrubPoller;
   std::unique_ptr<ScrubPoller> mPoller;
};

// Specialist in drawing the scrub speed, and listening for certain events
class ScrubbingOverlay final : public wxEvtHandler, public Overlay
{
public:
   ScrubbingOverlay(AudacityProject *project);
   virtual ~ScrubbingOverlay();

private:
   std::pair<wxRect, bool> DoGetRectangle(wxSize size) override;
   void Draw(OverlayPanel &panel, wxDC &dc) override;

   void OnTimer(wxCommandEvent &event);

   const Scrubber &GetScrubber() const;
   Scrubber &GetScrubber();

   AudacityProject *mProject;

   wxRect mLastScrubRect, mNextScrubRect;
   wxString mLastScrubSpeedText, mNextScrubSpeedText;
};

#endif
