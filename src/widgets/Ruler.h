/**********************************************************************

  Audacity: A Digital Audio Editor

  Ruler.h

  Dominic Mazzoni

**********************************************************************/

#ifndef __AUDACITY_RULER__
#define __AUDACITY_RULER__

#include "OverlayPanel.h"

#include "../MemoryX.h"
#include <wx/bitmap.h>
#include <wx/dc.h>
#include <wx/dcmemory.h>
#include <wx/event.h>
#include <wx/font.h>
#include <wx/window.h>
#include "../Experimental.h"

class ViewInfo;
class AudacityProject;
class TimeTrack;
class SnapManager;
class NumberScale;
class TrackList;
class ZoomInfo;

class AUDACITY_DLL_API Ruler {
 public:

   enum RulerFormat {
      IntFormat,
      RealFormat,
      RealLogFormat,
      TimeFormat,
      LinearDBFormat,
   };

   //
   // Constructor / Destructor
   //

   Ruler();
   ~Ruler();

   //
   // Required Ruler Parameters
   //

   void SetBounds(int left, int top, int right, int bottom);

   // wxHORIZONTAL || wxVERTICAL
   void SetOrientation(int orient);

   // min is the value at (x, y)
   // max is the value at (x+width, y+height)
   // (at the center of the pixel, in both cases)
   void SetRange(double min, double max);

   // An overload needed for the special case of fisheye
   // min is the value at (x, y)
   // max is the value at (x+width, y+height)
   // hiddenMin, hiddenMax are the values that would be shown without the fisheye.
   // (at the center of the pixel, in both cases)
   void SetRange(double min, double max, double hiddenMin, double hiddenMax);

   //
   // Optional Ruler Parameters
   //

   // If twoTone is true, cause zero and positive numbers to appear black, negative in another color.
   void SetTwoTone(bool twoTone);

   // IntFormat, RealFormat, or TimeFormat
   void SetFormat(RulerFormat format);

   // Specify the name of the units (like "dB") if you
   // want numbers like "1.6" formatted as "1.6 dB".
   void SetUnits(const wxString &units);

   // Logarithmic
   void SetLog(bool log);

   // Minimum number of pixels between labels
   void SetSpacing(int spacing);

   // If this is true, the edges of the ruler will always
   // receive a label.  If not, the nearest round number is
   // labeled (which may or may not be the edge).
   void SetLabelEdges(bool labelEdges);

   // Makes a vertical ruler hug the left side (instead of right)
   // and a horizontal ruler hug the top (instead of bottom)
   void SetFlip(bool flip);

   // Set it to false if you don't want minor labels.
   void SetMinor(bool value);

   // Good defaults are provided, but you can override here
   void SetFonts(const wxFont &minorFont, const wxFont &majorFont, const wxFont &minorMinorFont);
   struct Fonts { wxFont *major, *minor, *minorMinor; };
   Fonts GetFonts() const
   { return { mMajorFont, mMinorFont, mMinorMinorFont }; }

   // Copies *pScale if it is not NULL
   void SetNumberScale(const NumberScale *pScale);

   // The ruler will not draw text within this (pixel) range.
   // Use this if you have another graphic object obscuring part
   // of the ruler's area.  The values start and end are interpreted
   // relative to the Ruler's local coordinates.
   void OfflimitsPixels(int start, int end);

   //
   // Calculates and returns the maximum size required by the ruler
   //
   void GetMaxSize(wxCoord *width, wxCoord *height);


   // The following functions should allow a custom ruler setup:
   // autosize is a GREAT thing, but for some applications it's
   // useful the definition of a label array and label step by
   // the user.
   void SetCustomMode(bool value);
   // If this is the case, you should provide a wxString array of labels, start
   // label position, and labels step. The range eventually specified will be
   // ignored.
   void SetCustomMajorLabels(wxArrayString *label, int numLabel, int start, int step);
   void SetCustomMinorLabels(wxArrayString *label, int numLabel, int start, int step);

   void SetUseZoomInfo(int leftOffset, const ZoomInfo *zoomInfo);

   //
   // Drawing
   //

   // Note that it will not erase for you...
   void Draw(wxDC& dc);
   void Draw(wxDC& dc, const TimeTrack* timetrack);
   // If length <> 0, draws lines perpendiculars to ruler corresponding
   // to selected ticks (major, minor, or both), in an adjacent window.
   // You may need to use the offsets if you are using part of the dc for rulers, borders etc.
   void DrawGrid(wxDC& dc, int length, bool minor = true, bool major = true, int xOffset = 0, int yOffset = 0);

   // So we can have white ticks on black...
   void SetTickColour( const wxColour & colour)
   { mTickColour = colour; mPen.SetColour( colour );}

   // Force regeneration of labels at next draw time
   void Invalidate();

 private:
   void Update();
   void Update(const TimeTrack* timetrack);
   void FindTickSizes();
   void FindLinearTickSizes(double UPP);
   wxString LabelString(double d, bool major);

   void Tick(int pos, double d, bool major, bool minor);

   // Another tick generator for custom ruler case (noauto) .
   void TickCustom(int labelIdx, bool major, bool minor);

public:
   bool mbTicksOnly; // true => no line the length of the ruler
   bool mbTicksAtExtremes;
   wxRect mRect;

private:
   static wxColour mTickColour;
   wxPen mPen;

   int          mMaxWidth, mMaxHeight;
   int          mLeft, mTop, mRight, mBottom, mLead;
   int          mLength;
   int          mLengthOld;
   wxDC        *mDC;

   wxFont      *mMinorFont, *mMajorFont;
   wxFont      *mMinorMinorFont;
   bool         mUserFonts;

   double       mMin, mMax;
   double       mHiddenMin, mHiddenMax;

   double       mMajor;
   double       mMinor;

   int          mDigits;

   int         *mUserBits;
   int         *mBits;
   int          mUserBitLen;

   bool         mValid;

   class Label {
    public:
      double value;
      int pos;
      int lx, ly;
      wxString text;

      void Draw(wxDC &dc, bool twoTone) const;
   };

   int          mNumMajor;
   Label       *mMajorLabels;
   int          mNumMinor;
   Label       *mMinorLabels;
   int          mNumMinorMinor;
   Label       *mMinorMinorLabels;

   // Returns 'zero' label coordinate (for grid drawing)
   int FindZero(Label * label, int len);

   public:
   int GetZeroPosition();

   private:
   int          mOrientation;
   int          mSpacing;
   bool         mHasSetSpacing;
   bool         mLabelEdges;
   RulerFormat  mFormat;
   bool         mLog;
   bool         mFlip;
   bool         mCustom;
   bool         mbMinor;
   bool         mMajorGrid;      //  for grid drawing
   bool         mMinorGrid;      //         .
   int          mGridLineLength; //        end
   wxString     mUnits;
   bool         mTwoTone;
   const ZoomInfo *mUseZoomInfo;
   int          mLeftOffset;

   NumberScale *mpNumberScale;
};

class AUDACITY_DLL_API RulerPanel final : public wxPanel {
   DECLARE_DYNAMIC_CLASS(RulerPanel)

 public:
   RulerPanel(wxWindow* parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize);

   ~RulerPanel();

   void DoSetSize(int x, int y,
                  int width, int height,
                  int sizeFlags = wxSIZE_AUTO) override;

   void OnErase(wxEraseEvent &evt);
   void OnPaint(wxPaintEvent &evt);
   void OnSize(wxSizeEvent &evt);

   // We don't need or want to accept focus.
   bool AcceptsFocus() const { return false; }
   // So that wxPanel is not included in Tab traversal - see wxWidgets bug 15581
   bool AcceptsFocusFromKeyboard() const { return false; }

 public:

   Ruler  ruler;

private:
    DECLARE_EVENT_TABLE()
};

class QuickPlayIndicatorOverlay;
class QuickPlayRulerOverlay;

// This is an Audacity Specific ruler panel which additionally
// has border, selection markers, play marker.
// Once TrackPanel uses wxSizers, we will derive it from some
// wxWindow and the GetSize and SetSize functions
// will then be wxWidgets functions instead.
class AUDACITY_DLL_API AdornedRulerPanel final : public OverlayPanel
{
public:
   AdornedRulerPanel(AudacityProject* parent,
                     wxWindowID id,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     ViewInfo *viewinfo = NULL);

   ~AdornedRulerPanel();

#ifndef EXPERIMENTAL_TIME_RULER_NAVIGATION
   bool AcceptsFocus() const override { return false; }
#endif

public:
   static int GetRulerHeight();
   static int GetRulerHeight(bool showScrubBar);
   wxRect GetInnerRect() const { return mInner; }

   void SetLeftOffset(int offset);

   void DrawSelection();

   void SetPlayRegion(double playRegionStart, double playRegionEnd);
   void ClearPlayRegion();
   void GetPlayRegion(double* playRegionStart, double* playRegionEnd);

   void GetMaxSize(wxCoord *width, wxCoord *height);

   void InvalidateRuler();

   void UpdatePrefs();

   enum class StatusChoice {
      FirstButton = 0,
      QuickPlayButton = FirstButton,
      ScrubBarButton,

      NumButtons,
      LastButton = NumButtons - 1,
      NoButton = -1,

      EnteringQP = NumButtons,
      EnteringScrubZone,
      Leaving,
      NoChange
   };
   enum class PointerState {
      Out = 0, In, InArrow
   };
   struct CaptureState {
      CaptureState() {}
      CaptureState(StatusChoice s, PointerState p) : button(s), state(p) {}
      StatusChoice button { StatusChoice::NoButton };
      PointerState state { PointerState::Out };
   };

   friend inline StatusChoice &operator++ (StatusChoice &choice) {
      choice = static_cast<StatusChoice>(1 + static_cast<int>(choice));
      return choice;
   }
   friend inline StatusChoice &operator-- (StatusChoice &choice) {
      choice = static_cast<StatusChoice>(-1 + static_cast<int>(choice));
      return choice;
   }

   void RegenerateTooltips(StatusChoice choice);

   void ShowQuickPlayIndicator();
   void HideQuickPlayIndicator();
   void UpdateQuickPlayPos(wxCoord &mousPosX);

private:
   void OnCapture(wxCommandEvent & evt);
   void OnPaint(wxPaintEvent &evt);
   void OnSize(wxSizeEvent &evt);
   void UpdateRects();
   void OnMouseEvents(wxMouseEvent &evt);
   void HandleQPDoubleClick(wxMouseEvent &event, wxCoord mousePosX);
   void HandleQPClick(wxMouseEvent &event, wxCoord mousePosX);
   void HandleQPDrag(wxMouseEvent &event, wxCoord mousePosX);
   void HandleQPRelease(wxMouseEvent &event);
   void StartQPPlay(bool looped, bool cutPreview);

   static inline bool IsButton(StatusChoice choice)
   {
      auto integer = static_cast<int>(choice);
      return integer >= 0 &&
         integer < static_cast<int>(StatusChoice::NumButtons);
   }
   static inline bool IsButton(int choice)
   { return IsButton(static_cast<StatusChoice>(choice)); }

   void UpdateStatusBarAndTooltips(StatusChoice choice);

   void OnCaptureLost(wxMouseCaptureLostEvent &evt);

   void DoDrawBackground(wxDC * dc);
   void DoDrawEdge(wxDC *dc);
   void DoDrawMarks(wxDC * dc, bool /*text */ );
   void DoDrawSelection(wxDC * dc);
public:
   void DoDrawIndicator(wxDC * dc, wxCoord xx, bool playing, int width, bool scrub);

private:
   QuickPlayIndicatorOverlay *GetOverlay();
   void ShowOrHideQuickPlayIndicator(bool show);
   void DoDrawPlayRegion(wxDC * dc);

   wxRect GetButtonAreaRect(bool includeBorder = false) const;

   struct ButtonStrings {
      wxString label, enable, disable;
   };
   static const ButtonStrings PushbuttonLabels[];
   static const ButtonStrings *GetPushButtonStrings(StatusChoice choice)
   {
      if(IsButton(choice))
         return &PushbuttonLabels[static_cast<size_t>(choice)];
      return nullptr;
   }

   wxRect GetButtonRect( StatusChoice button ) const;
   PointerState InButtonRect( StatusChoice button, wxMouseEvent *pEvent ) const;
   CaptureState FindButton( wxMouseEvent &mouseEvent ) const;
   bool GetButtonState( StatusChoice button ) const;
   void ToggleButtonState( StatusChoice button );
   void ShowButtonMenu( StatusChoice button, const wxPoint *pPosition);
   void DoDrawPushbutton
      (wxDC *dc, StatusChoice button, bool buttonState, bool arrowState) const;
   void DoDrawPushbuttons(wxDC *dc) const;
   void HandlePushbuttonClick(wxMouseEvent &evt);
   void HandlePushbuttonEvent(wxMouseEvent &evt);

   wxFont &GetButtonFont() const;

   double Pos2Time(int p, bool ignoreFisheye = false);
   int Time2Pos(double t, bool ignoreFisheye = false);

   bool IsWithinMarker(int mousePosX, double markerTime);

private:

   wxCursor mCursorDefault;
   wxCursor mCursorHand;
   wxCursor mCursorSizeWE;
   bool mIsWE;

   Ruler mRuler;
   ViewInfo *const mViewInfo;
   AudacityProject *const mProject;
   TrackList *mTracks;

   wxRect mOuter;
   wxRect mScrubZone;
   wxRect mInner;

   int mLeftOffset;  // Number of pixels before we hit the 'zero position'.


   double mIndTime;
   double mQuickPlayPos;

   SnapManager *mSnapManager;
   bool mIsSnapped;

   bool   mPlayRegionLock;
   double mPlayRegionStart;
   double mPlayRegionEnd;
   double mOldPlayRegionStart;
   double mOldPlayRegionEnd;

   bool mIsRecording;

   //
   // Pop-up menu
   //
   void ShowMenu(const wxPoint & pos);
   void ShowScrubMenu(const wxPoint & pos);
   void DragSelection();
   void HandleSnapping();
   void OnToggleQuickPlay(wxCommandEvent &evt);
   void OnSyncSelToQuickPlay(wxCommandEvent &evt);
   void OnTimelineToolTips(wxCommandEvent &evt);
   void OnAutoScroll(wxCommandEvent &evt);
   void OnLockPlayRegion(wxCommandEvent &evt);

   void OnToggleScrubbing(wxCommandEvent&);

   void OnCaptureKey(wxCommandEvent &event);
   void OnKeyDown(wxKeyEvent &event);
   void OnSetFocus(wxFocusEvent &);
   void OnKillFocus(wxFocusEvent &);
   void OnContextMenu(wxContextMenuEvent & WXUNUSED(event));

   bool mPlayRegionDragsSelection;
   bool mTimelineToolTip;
   bool mQuickPlayEnabled;

   CaptureState mCaptureState {};

   enum MouseEventState {
      mesNone,
      mesDraggingPlayRegionStart,
      mesDraggingPlayRegionEnd,
      mesSelectingPlayRegionClick,
      mesSelectingPlayRegionRange
   };

   MouseEventState mMouseEventState;
   double mLeftDownClick;  // click position in seconds
   int mLastMouseX;  // Pixel position
   bool mIsDragging;

   std::unique_ptr<QuickPlayIndicatorOverlay> mOverlay;

   StatusChoice mPrevZone { StatusChoice::NoChange };

   struct TabState {
      StatusChoice mButton { StatusChoice::FirstButton };
      bool mMenu { false };

      TabState() {}
      TabState(StatusChoice button, bool menu)
         : mButton{ button }, mMenu{ menu } {}

      bool operator == (const TabState &rhs) const
         { return mButton == rhs.mButton && mMenu == rhs.mMenu; }
      bool operator != (const TabState &rhs) const { return !(*this == rhs); }

      TabState &operator ++ () {
         if (!mMenu)
            mMenu = true;
         else {
            mMenu = false;
            if (!IsButton (++mButton))
               mButton = StatusChoice::FirstButton;
         }
         return *this;
      }

      TabState &operator -- () {
         if (mMenu)
            mMenu = false;
         else {
            mMenu = true;
            if (!IsButton (--mButton))
               mButton = StatusChoice::LastButton;
         }
         return *this;
      }
   };
   TabState mTabState;

   bool mShowScrubbing { true };

   mutable int mButtonFontSize { -1 };
   mutable wxFont mButtonFont;

   bool mDoubleClick {};
   bool mShowingMenu {};

   DECLARE_EVENT_TABLE()

   friend QuickPlayRulerOverlay;
};

#endif //define __AUDACITY_RULER__
