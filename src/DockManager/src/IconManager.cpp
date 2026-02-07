#include "IconManager.h"

#include <QApplication>
#include <QWidget>
#include <QStyle>
#include <QFileIconProvider>

namespace DockManager::Icons {

namespace {

QStyle* activeStyle(const QWidget* context)
{
    if (context) {
        return context->style();
    }
    if (qApp) {
        return qApp->style();
    }

    return nullptr;
}

} // namespace

QIcon icon(Id id, const QWidget* context)
{
    QStyle* style = activeStyle(context);
    if (!style) {
        return QIcon();
    }

    QFileIconProvider fileIcons;

    switch (id) {
    case Id::Import:
        return style->standardIcon(QStyle::SP_DialogOpenButton);
    case Id::Save:
        return style->standardIcon(QStyle::SP_DialogSaveButton);
    case Id::AddGroup:
        return fileIcons.icon(QFileIconProvider::Folder);
    case Id::AddFeature:
        return style->standardIcon(QStyle::SP_FileDialogContentsView);
    case Id::AddTest:
        return fileIcons.icon(QFileIconProvider::File);
    case Id::Remove:
        return style->standardIcon(QStyle::SP_TrashIcon);
    case Id::ExpandAll:
        return style->standardIcon(QStyle::SP_ArrowDown);
    case Id::CollapseAll:
        return style->standardIcon(QStyle::SP_ArrowUp);
    case Id::PanelExplorer:
        return style->standardIcon(QStyle::SP_DirOpenIcon);
    case Id::PanelProgress:
        return style->standardIcon(QStyle::SP_FileDialogDetailedView);
    case Id::GroupItem:
        return fileIcons.icon(QFileIconProvider::Folder);
    case Id::FeatureItem:
        return style->standardIcon(QStyle::SP_FileDialogContentsView);
    case Id::TestItem:
        return fileIcons.icon(QFileIconProvider::File);
    case Id::Run:
        return style->standardIcon(QStyle::SP_MediaPlay);
    }

    return QIcon();
}

} // namespace DockManager::Icons
