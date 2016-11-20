#include "pncore/corpus/Corpus.h"
#include "pncore/corpus/CorpusParticipation.h"
#include "pncore/structure/MetadataStructure.h"
using namespace Praaline::Core;

#include "CorpusParticipationTableModel.h"

struct CorpusParticipationTableModelData {
    CorpusParticipationTableModelData() : multiCorpus(false) {}

    bool multiCorpus;
    QList<QPointer<CorpusParticipation> > items;
    QList<QPointer<MetadataStructureAttribute> > attributes;
    QPointer<Corpus> corpus;
};


CorpusParticipationTableModel::CorpusParticipationTableModel(QList<QPointer<CorpusParticipation> > items,
                                                             QList<QPointer<MetadataStructureAttribute> > attributes,
                                                             QPointer<Corpus> corpus, bool multiCorpus, QObject *parent)
    : QAbstractTableModel(parent), d(new CorpusParticipationTableModelData)
{
    d->items = items;
    d->attributes = attributes;
    d->corpus = corpus;
    d->multiCorpus = multiCorpus;
}

CorpusParticipationTableModel::~CorpusParticipationTableModel()
{
    delete d;
}

QModelIndex CorpusParticipationTableModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int CorpusParticipationTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return d->items.count();
}

int CorpusParticipationTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    int basicPropertiesCount = (d->multiCorpus) ? 4 : 3;
    return basicPropertiesCount + d->attributes.count();
}

QVariant CorpusParticipationTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Horizontal) {
        if (d->multiCorpus) {
            if (section == 0) return tr("Corpus ID");
            else if (section == 1) return tr("Communication ID");
            else if (section == 2) return tr("Speaker ID");
            else if (section == 3) return tr("Role");
            else if ((section - 4) >= 0 && (section - 4) < d->attributes.count())
                return d->attributes.at(section - 4)->name();
            else
                return QVariant();
        } else {
            if (section == 0) return tr("Communication ID");
            else if (section == 1) return tr("Speaker ID");
            else if (section == 2) return tr("Role");
            else if ((section - 3) >= 0 && (section - 3) < d->attributes.count())
                return d->attributes.at(section - 3)->name();
            else
                return QVariant();
        }
    }
    return QVariant();
}

QVariant CorpusParticipationTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (index.row() >= d->items.count() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        QPointer<CorpusParticipation> part = d->items.at(index.row());
        if (!part) return QVariant();
        if (d->multiCorpus) {
            if (index.column() == 0) return part->property("corpusID");
            else if (index.column() == 1) return part->property("communicationID");
            else if (index.column() == 2) return part->property("speakerID");
            else if (index.column() == 3) return part->property("role");
            else if ((index.column() - 4) >= 0 && (index.column() - 4) < d->attributes.count())
                return part->property(d->attributes.at(index.column() - 4)->ID());
            else
                return QVariant();
        } else {
            if (index.column() == 0) return part->property("communicationID");
            else if (index.column() == 1) return part->property("speakerID");
            else if (index.column() == 2) return part->property("role");
            else if ((index.column() - 3) >= 0 && (index.column() - 3) < d->attributes.count())
                return part->property(d->attributes.at(index.column() - 3)->ID());
            else
                return QVariant();
        }
    }
    return QVariant();
}


Qt::ItemFlags CorpusParticipationTableModel::flags(const QModelIndex &index) const
{
    if (index.column() < columnCount())
        return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return QAbstractTableModel::flags(index);
}

bool CorpusParticipationTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    if (index.row() >= d->items.count() || index.row() < 0)
        return false;

    if (role == Qt::EditRole) {
        QPointer<CorpusParticipation> part = d->items.at(index.row());
        if (!part) return false;
        if (d->multiCorpus) {
            if      (index.column() == 0) return part->setProperty("corpusID", value);
            else if (index.column() == 1) return part->setProperty("communicationID", value);
            else if (index.column() == 2) return part->setProperty("speakerID", value);
            else if (index.column() == 3) return part->setProperty("role", value);
            else if ((index.column() - 4) >= 0 && (index.column() - 4) < d->attributes.count())
                return part->setProperty(d->attributes.at(index.column() - 4)->ID(), value);
            else
                return false;
        } else {
            if      (index.column() == 0) return part->setProperty("communicationID", value);
            else if (index.column() == 1) return part->setProperty("speakerID", value);
            else if (index.column() == 2) return part->setProperty("role", value);
            else if ((index.column() - 3) >= 0 && (index.column() - 3) < d->attributes.count())
                return part->setProperty(d->attributes.at(index.column() - 3)->ID(), value);
            else
                return false;
        }
    }
    return QAbstractTableModel::setData(index, value, role);
}

bool CorpusParticipationTableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    return false;
}

bool CorpusParticipationTableModel::removeRows(int position, int rows, const QModelIndex &index)
{
    return false;
}
