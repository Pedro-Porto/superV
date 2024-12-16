#include <stdio.h>
#include <limits.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <assert.h>


Bool PrintSelection(Display *display, Window window, const char *bufname, const char *fmtname)
{
  char *result;
  unsigned long ressize, restail;
  int resbits;
  Atom bufid = XInternAtom(display, bufname, False),
       fmtid = XInternAtom(display, fmtname, False),
       propid = XInternAtom(display, "XSEL_DATA", False),
       incrid = XInternAtom(display, "INCR", False);
  XEvent event;

  XSelectInput (display, window, PropertyChangeMask);
  XConvertSelection(display, bufid, fmtid, propid, window, CurrentTime);
  do {
    XNextEvent(display, &event);
  } while (event.type != SelectionNotify || event.xselection.selection != bufid);

  if (event.xselection.property)
  {
    XGetWindowProperty(display, window, propid, 0, LONG_MAX/4, True, AnyPropertyType,
      &fmtid, &resbits, &ressize, &restail, (unsigned char**)&result);
    if (fmtid != incrid)
      printf("%.*s", (int)ressize, result);
    XFree(result);

    if (fmtid == incrid)
      do {
        do {
          XNextEvent(display, &event);
        } while (event.type != PropertyNotify || event.xproperty.atom != propid || event.xproperty.state != PropertyNewValue);

        XGetWindowProperty(display, window, propid, 0, LONG_MAX/4, True, AnyPropertyType,
          &fmtid, &resbits, &ressize, &restail, (unsigned char**)&result);
        printf("%.*s", (int)ressize, result);
        XFree(result);
      } while (ressize > 0);

    return True;
  }
  else 
    return False;
}

void WatchSelection(Display *display, Window window, const char *bufname)
{
  int event_base, error_base;
  XEvent event;
  Atom bufid = XInternAtom(display, bufname, False);

  assert( XFixesQueryExtension(display, &event_base, &error_base) );
  XFixesSelectSelectionInput(display, DefaultRootWindow(display), bufid, XFixesSetSelectionOwnerNotifyMask);

  while (True)
  {
    XNextEvent(display, &event);

    if (event.type == event_base + XFixesSelectionNotify &&
        ((XFixesSelectionNotifyEvent*)&event)->selection == bufid)
    {
      if (!PrintSelection(display, window, bufname, "UTF8_STRING"))
        PrintSelection(display, window, bufname, "STRING");

      fflush(stdout);
    }
  }
}


int main()
{
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Unable to open X display\n");
        return 1;
    }

    unsigned long color = BlackPixel(display, DefaultScreen(display));
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 1, 1, 0, color, color);

    WatchSelection(display, window, "CLIPBOARD");

    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}