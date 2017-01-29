#include "CorpusExplorerTableWidget.h"
#include "ui_CorpusExplorerTableWidget.h"

#include "grid/qadvancedtableview.h"
#include "grid/qadvancedheaderview.h"
#include "grid/qconditionaldecorationdialog.h"
#include "grid/qconditionaldecorationproxymodel.h"
#include "grid/qfiltermodel.h"
#include "grid/qfiltermodelproxy.h"
#include "grid/qfilterviewconnector.h"
#include "grid/qgroupingproxymodel.h"
#include "grid/qconditionaldecoration.h"
#include "grid/qfixedrowstableview.h"
#include "grid/qmimedatautil.h"
#include "grid/qrangefilter.h"
#include "grid/quniquevaluesproxymodel.h"
#include "grid/qselectionlistfilter.h"
#include "grid/qtextfilter.h"
#include "grid/qvaluefilter.h"

#include "pncore/corpus/CorpusObject.h"
#include "pncore/corpus/Corpus.h"
#include "pncore/corpus/CorpusCommunication.h"
#include "pncore/datastore/CorpusRepository.h"
#include "pncore/datastore/MetadataDatastore.h"
#include "pncore/structure/MetadataStructure.h"
using namespace Praaline::Core;

#include "pngui/model/corpus/CorpusCommunicationTableModel.h"
#include "pngui/model/corpus/CorpusSpeakerTableModel.h"
#include "pngui/model/corpus/CorpusRecordingTableModel.h"
#include "pngui/model/corpus/CorpusAnnotationTableModel.h"
#include "pngui/model/corpus/CorpusParticipationTableModel.h"
#include "pngui/widgets/SelectionListDataProviderProxy.h"
#include "pngui/widgets/GridViewWidget.h"

#include "CorpusRepositoriesManager.h"

struct CorpusExplorerTableWidgetData
{
    CorpusExplorerTableWidgetData() :
        corpusRepositoryManager(0), corpuObjectType(CorpusObject::Type_Communication), model(0)
    {}

    CorpusRepositoriesManager *corpusRepositoryManager;
    CorpusObject::Type corpuObjectType;
    QAbstractTableModel *model;

    GridViewWidget *tableView;
//    QToolBar *toolbarCorpusExplorerTable;
};

CorpusExplorerTableWidget::CorpusExplorerTableWidget(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::CorpusExplorerTableWidget), d(new CorpusExplorerTableWidgetData)
{
    ui->setupUi(this);

    // Get Corpus Repositories Manager from global object list
    QList<QObject *> list;
    list = OBJECT_MANAGER->registeredInterfaces("CorpusRepositoriesManager");
    foreach (QObject* obj, list) {
        CorpusRepositoriesManager *manager = qobject_cast<CorpusRepositoriesManager *>(obj);
        if (manager) d->corpusRepositoryManager = manager;
    }
    connect(d->corpusRepositoryManager, SIGNAL(activeCorpusRepositoryChanged(QString)), this, SLOT(activeCorpusRepositoryChanged(QString)));
    // Add grid
    d->tableView = new GridViewWidget(this);
    ui->gridLayoutTable->addWidget(d->tableView);
    d->tableView->tableView()->verticalHeader()->setDefaultSectionSize(20);
    // Corpus object types
    ui->comboItemType->addItems(QStringList() << tr("Communications") << tr("Speakers") << tr("Recordings") << tr("Annotations") << tr("Participations"));
    ui->comboItemType->setCurrentText(tr("Communications"));
    connect(ui->comboItemType, SIGNAL(currentTextChanged(QString)), this, SLOT(corpusObjectTypeChanged(QString)));
    // Menu and Toolbar actions
//    d->toolbarCorpusExplorerTable = new QToolBar("Corpus Explorer Tables", this);
//    d->toolbarCorpusExplorerTable->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
//    d->toolbarCorpusExplorerTable->setIconSize(QSize(24, 24));
//    this->addToolBar(d->toolbarCorpusExplorerTable);
}

CorpusExplorerTableWidget::~CorpusExplorerTableWidget()
{
    delete ui;
    delete d;
}

void CorpusExplorerTableWidget::corpusObjectTypeChanged(const QString &text)
{
    if (text == tr("Communications"))
        d->corpuObjectType = CorpusObject::Type_Communication;
    else if (text == tr("Speakers"))
        d->corpuObjectType = CorpusObject::Type_Speaker;
    else if (text == tr("Recordings"))
        d->corpuObjectType = CorpusObject::Type_Recording;
    else if (text == tr("Annotations"))
        d->corpuObjectType = CorpusObject::Type_Annotation;
    else if (text == tr("Participations"))
        d->corpuObjectType = CorpusObject::Type_Participation;
    refreshModel();
}

void CorpusExplorerTableWidget::refreshModel()
{
    QPointer<CorpusRepository> repository = d->corpusRepositoryManager->activeCorpusRepository();
    if (!repository) {
        d->tableView->tableView()->setModel(0);
        if (d->model) delete d->model;
        d->model = 0;
        return;
    }
    // otherwise, another corpus has been selected
    QAbstractTableModel *newModel = 0;
    if (d->corpuObjectType == CorpusObject::Type_Communication) {
        QList<QPointer<MetadataStructureAttribute> > attributes = repository->metadataStructure()->attributes(CorpusObject::Type_Communication);
        QList<QPointer<CorpusCommunication> > items = repository->metadata()->getCommunications(MetadataDatastore::Selection("", "", ""));
        newModel = new CorpusCommunicationTableModel(items, attributes, true);
    }
    else if (d->corpuObjectType == CorpusObject::Type_Speaker) {
        QList<QPointer<MetadataStructureAttribute> > attributes = repository->metadataStructure()->attributes(CorpusObject::Type_Speaker);
        QList<QPointer<CorpusSpeaker> > items = repository->metadata()->getSpeakers(MetadataDatastore::Selection("", "", ""));
        newModel = new CorpusSpeakerTableModel(items, attributes, true);
    }
    else if (d->corpuObjectType == CorpusObject::Type_Recording) {
        QList<QPointer<MetadataStructureAttribute> > attributes = repository->metadataStructure()->attributes(CorpusObject::Type_Recording);
        QList<QPointer<CorpusRecording> > items = repository->metadata()->getRecordings(MetadataDatastore::Selection("", "", ""));
        newModel = new CorpusRecordingTableModel(items, attributes, true);
    }
    else if (d->corpuObjectType == CorpusObject::Type_Annotation) {
        QList<QPointer<MetadataStructureAttribute> > attributes = repository->metadataStructure()->attributes(CorpusObject::Type_Annotation);
        QList<QPointer<CorpusAnnotation> > items = repository->metadata()->getAnnotations(MetadataDatastore::Selection("", "", ""));
        newModel = new CorpusAnnotationTableModel(items, attributes, true);
    }
    else if (d->corpuObjectType == CorpusObject::Type_Participation) {
        QList<QPointer<MetadataStructureAttribute> > attributes = repository->metadataStructure()->attributes(CorpusObject::Type_Participation);
        QList<QPointer<CorpusParticipation> > items = repository->metadata()->getParticipations(MetadataDatastore::Selection("", "", ""));
        newModel = new CorpusParticipationTableModel(items, attributes, true);
    }
    // try to update model, if not possible show an empty table
    if (newModel) {
        d->tableView->tableView()->setModel(newModel);
        d->tableView->tableView()->horizontalHeader()->setSectionsClickable(true);
        d->tableView->tableView()->setDefaultFilterType(0, QTextFilter::Type);
        connect(d->tableView->tableView()->filterProxyModel(), SIGNAL(resultCountChanged(int,int)),
                this, SLOT(resultChanged(int,int)));
        connect(d->tableView->tableView()->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
                this, SLOT(selectionChanged(QItemSelection, QItemSelection)));

        if (d->model) delete d->model;
        d->model = newModel;
    }
    else {
        d->tableView->tableView()->setModel(0);
        if (d->model) delete d->model;
        d->model = 0;
    }
}

void CorpusExplorerTableWidget::activeCorpusRepositoryChanged(const QString &repositoryID)
{
    Q_UNUSED(repositoryID);
    refreshModel();
}

void CorpusExplorerTableWidget::resultChanged(int filterRows, int unfilteredRows)
{
    if (unfilteredRows - filterRows > 0)
        d->tableView->setResultsLabelText(tr("Result: %1 of %2").arg(filterRows).arg(unfilteredRows));
    else
        d->tableView->setResultsLabelText(tr("All items: %1").arg(filterRows));
}

void CorpusExplorerTableWidget::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{

}

void CorpusExplorerTableWidget::contextMenuRequested(const QPoint & point)
{

}
