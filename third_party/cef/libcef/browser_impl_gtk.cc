// Copyright (c) 2011 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2006-2008 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cef_context.h"
#include "browser_impl.h"
#include "browser_settings.h"

#include <gtk/gtk.h>

#include "third_party/WebKit/Source/WebKit/chromium/public/WebFrame.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebRect.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebSize.h"
#include "third_party/WebKit/Source/WebKit/chromium/public/WebView.h"
#include "webkit/glue/webpreferences.h"

using WebKit::WebRect;
using WebKit::WebSize;

void BrowserParentDestroyed(GtkWidget *window, CefBrowserImpl* browser)
{	
  // Destroy the browser.
  browser->UIT_DestroyBrowser();
}

void CefBrowserImpl::ParentWindowWillClose()
{
  // TODO(port): Implement this method if necessary.
}

CefWindowHandle CefBrowserImpl::GetWindowHandle()
{
  AutoLock lock_scope(this);
  return window_info_.m_Widget;
}

bool CefBrowserImpl::IsWindowRenderingDisabled()
{
  // TODO(port): Add support for off-screen rendering.
  return false;
}

gfx::NativeView CefBrowserImpl::UIT_GetMainWndHandle() {
  REQUIRE_UIT();
  GtkWidget* toplevel = gtk_widget_get_ancestor((GtkWidget*)window_info_.m_Widget,
      GTK_TYPE_WINDOW);
  return GTK_IS_WIDGET(toplevel) ? GTK_WIDGET(toplevel) : NULL;
}

void CefBrowserImpl::UIT_CreateBrowser(const CefString& url)
{
  REQUIRE_UIT();
  Lock();
  
  if (!settings_.developer_tools_disabled)
    dev_tools_agent_.reset(new BrowserDevToolsAgent());

  // Add a reference that will be released in UIT_DestroyBrowser().
  AddRef();

  // Add the new browser to the list maintained by the context
  _Context->AddBrowser(this);

  WebPreferences prefs;
  BrowserToWebSettings(settings_, prefs);

  // Create the webview host object
  webviewhost_.reset(
      WebViewHost::Create((GtkWidget*)window_info_.m_ParentWidget, delegate_.get(), dev_tools_agent_.get(), prefs)
  );
  
  if (!settings_.developer_tools_disabled)
    dev_tools_agent_->SetWebView(webviewhost_->webview());

  window_info_.m_Widget = webviewhost_->view_handle();

  Unlock();

  g_signal_connect(GTK_WIDGET(window_info_.m_ParentWidget), "destroy", 
		   G_CALLBACK(BrowserParentDestroyed), this);

  if (client_.get()) {
    CefRefPtr<CefLifeSpanHandler> handler = client_->GetLifeSpanHandler();
    if(handler.get()) {
      // Notify the handler that we're done creating the new window
      handler->OnAfterCreated(this);
    }
  }

  if(url.length() > 0)
    UIT_LoadURL(GetMainFrame(), url);
}

void CefBrowserImpl::UIT_SetFocus(WebWidgetHost* host, bool enable)
{
  REQUIRE_UIT();
  if (!host)
    return;

  if(enable)
    gtk_widget_grab_focus(host->view_handle());
}

bool CefBrowserImpl::UIT_ViewDocumentString(WebKit::WebFrame *frame)
{
  REQUIRE_UIT();

  char buff[] = "/tmp/CEFSourceXXXXXX";
  int fd = mkstemp(buff);

  if (fd == -1)
    return false;

  FILE* srcOutput;
  srcOutput = fdopen(fd, "w+");

  if (!srcOutput)
    return false;
 
  std::string markup = frame->contentAsMarkup().utf8();
  if (fputs(markup.c_str(), srcOutput) < 0)
  {
    fclose(srcOutput);
	return false;
  }

  fclose(srcOutput);
  std::string newName(buff);
  newName.append(".txt");
  if (rename(buff, newName.c_str()) != 0)
    return false;

  std::string openCommand("xdg-open ");
  openCommand += newName;

  if (system(openCommand.c_str()) != 0)
    return false;

  return true;
}

void CefBrowserImpl::UIT_PrintPage(int page_number, int total_pages,
                                   const gfx::Size& canvas_size,
                                   WebKit::WebFrame* frame) {
  REQUIRE_UIT();

  // TODO(port): Add implementation.
  NOTIMPLEMENTED();
}

void CefBrowserImpl::UIT_PrintPages(WebKit::WebFrame* frame) {
  REQUIRE_UIT();

  // TODO(port): Add implementation.
  NOTIMPLEMENTED();
}

int CefBrowserImpl::UIT_GetPagesCount(WebKit::WebFrame* frame)
{
  REQUIRE_UIT();

  // TODO(port): Add implementation.
  NOTIMPLEMENTED();
  return 0;
}

// static
void CefBrowserImpl::UIT_CloseView(gfx::NativeView view)
{
  if (!view)
    return;
    
  GtkWidget* window = gtk_widget_get_parent(gtk_widget_get_parent(GTK_WIDGET(view)));
  
  if (!window)
    return;
    
  MessageLoop::current()->PostTask(FROM_HERE, NewRunnableFunction(
      &gtk_widget_destroy, GTK_WIDGET(window)));
}

// static
bool CefBrowserImpl::UIT_IsViewVisible(gfx::NativeView view)
{
  if (!view)
    return false;
    
  if (view->window)
    return (bool)gdk_window_is_visible(view->window);
  else
    return false;
}