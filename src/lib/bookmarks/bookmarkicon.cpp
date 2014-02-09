/* ============================================================
* QupZilla - WebKit based browser
* Copyright (C) 2010-2014  David Rosca <nowrep@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */
#include "bookmarkicon.h"
#include "bookmarkswidget.h"
#include "bookmarks.h"
#include "mainapplication.h"
#include "webview.h"
#include "locationbar.h"
#include "pluginproxy.h"
#include "speeddial.h"

#include <QStyle>
#include <QContextMenuEvent>

BookmarkIcon::BookmarkIcon(QWidget* parent)
    : ClickableLabel(parent)
    , m_view(0)
    , m_bookmark(0)
{
    setObjectName("locationbar-bookmarkicon");
    setCursor(Qt::PointingHandCursor);
    setToolTip(tr("Bookmark this Page"));
    setFocusPolicy(Qt::ClickFocus);

    connect(mApp->bookmarks(), SIGNAL(bookmarkAdded(BookmarkItem*)), this, SLOT(bookmarksChanged()));
    connect(mApp->bookmarks(), SIGNAL(bookmarkRemoved(BookmarkItem*)), this, SLOT(bookmarksChanged()));
    connect(mApp->bookmarks(), SIGNAL(bookmarkChanged(BookmarkItem*)), this, SLOT(bookmarksChanged()));
    connect(mApp->plugins()->speedDial(), SIGNAL(pagesChanged()), this, SLOT(speedDialChanged()));

    connect(this, SIGNAL(clicked(QPoint)), this, SLOT(iconClicked()));
}

void BookmarkIcon::setWebView(WebView* view)
{
    m_view = view;
}

void BookmarkIcon::checkBookmark(const QUrl &url, bool forceCheck)
{
    if (!forceCheck && m_lastUrl == url) {
        return;
    }

    QList<BookmarkItem*> items = mApp->bookmarks()->searchBookmarks(url);
    m_bookmark = items.isEmpty() ? 0 : items.first();

    if (m_bookmark || !mApp->plugins()->speedDial()->pageForUrl(url).url.isEmpty()) {
        setBookmarkSaved();
    }
    else {
        setBookmarkDisabled();
    }

    m_lastUrl = url;
}

void BookmarkIcon::bookmarksChanged()
{
    checkBookmark(m_lastUrl, true);
}

void BookmarkIcon::speedDialChanged()
{
    checkBookmark(m_lastUrl, true);
}

void BookmarkIcon::iconClicked()
{
    if (!m_view || m_view->url().scheme() == QLatin1String("qupzilla")) {
        return;
    }

    BookmarksWidget* widget = new BookmarksWidget(m_view, m_bookmark, parentWidget());
    widget->showAt(parentWidget());
}

void BookmarkIcon::setBookmarkSaved()
{
    setProperty("bookmarked", QVariant(true));
    style()->unpolish(this);
    style()->polish(this);
    setToolTip(tr("Edit this bookmark"));
}

void BookmarkIcon::setBookmarkDisabled()
{
    setProperty("bookmarked", QVariant(false));
    style()->unpolish(this);
    style()->polish(this);
    setToolTip(tr("Bookmark this Page"));
}

void BookmarkIcon::contextMenuEvent(QContextMenuEvent* ev)
{
    // Prevent propagating to LocationBar
    ev->accept();
}

void BookmarkIcon::mousePressEvent(QMouseEvent* ev)
{
    ClickableLabel::mousePressEvent(ev);

    // Prevent propagating to LocationBar
    ev->accept();
}
