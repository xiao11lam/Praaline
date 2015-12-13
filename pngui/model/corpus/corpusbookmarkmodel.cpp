#include "corpusbookmarkmodel.h"

CorpusBookmarkModel::CorpusBookmarkModel(QList<QPointer<CorpusBookmark> > bookmarks, QObject *parent) :
    QAbstractTableModel(parent), m_bookmarks(bookmarks)
{
}

int CorpusBookmarkModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_bookmarks.count();
}

int CorpusBookmarkModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 6;
}

QVariant CorpusBookmarkModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0: return tr("Corpus ID"); break;
        case 1: return tr("Communication ID"); break;
        case 2: return tr("Annotation ID"); break;
        case 3: return tr("Time"); break;
        case 4: return tr("Name"); break;
        case 5: return tr("Notes"); break;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QVariant CorpusBookmarkModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() >= m_bookmarks.count() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        QPointer<CorpusBookmark> bookmark = m_bookmarks.at(index.row());
        if (!bookmark) return QVariant();
        switch (index.column()) {
        case 0: return bookmark->corpusID(); break;
        case 1: return bookmark->communicationID(); break;
        case 2: return bookmark->annotationID(); break;
        case 3: return bookmark->time().toDouble(); break;
        case 4: return bookmark->name(); break;
        case 5: return bookmark->notes(); break;
        default:
            return QVariant();
        }
    }
    return QVariant();
}

Qt::ItemFlags CorpusBookmarkModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    if (index.column() >= 4)
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    return QAbstractTableModel::flags(index);
}

bool CorpusBookmarkModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) return false;
    if (role != Qt::EditRole) return false;
    if (index.row() >= m_bookmarks.count() || index.row() < 0) return false;
    QPointer<CorpusBookmark> bookmark = m_bookmarks.at(index.row());
    if (!bookmark) return false;
    if (index.column() == 4) {
        bookmark->setName(value.toString());
        emit(dataChanged(index, index));
        return true;
    }
    if (index.column() == 5) {
        bookmark->setNotes(value.toString());
        emit(dataChanged(index, index));
        return true;
    }
    return false;
}
