#include <QApplication>
#include <QProgressBar>
#include <QStandardItem>
#include <QStandardItemModel>

#include <QtCharts/QChartView>
#include <QtCharts/QBoxPlotSeries>
#include <QtCharts/QBoxSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
QT_CHARTS_USE_NAMESPACE

#include "pncore/corpus/Corpus.h"
#include "pncore/corpus/CorpusCommunication.h"
#include "pncore/corpus/CorpusSpeaker.h"
#include "pncore/structure/MetadataStructure.h"
#include "pncore/datastore/CorpusRepository.h"
#include "pncore/datastore/MetadataDatastore.h"
#include "pncore/statistics/StatisticalSummary.h"
using namespace Praaline::Core;

#include "pngui/widgets/GridViewWidget.h"

#include "AnalyserTemporal.h"
#include "AnalyserTemporalItem.h"
#include "AnalyserTemporalWidget.h"
#include "ui_AnalyserTemporalWidget.h"

namespace Praaline {
namespace Plugins {
namespace StatisticsPluginTemporal {

struct AnalyserTemporalWidgetData {
    AnalyserTemporalWidgetData() :
        repository(0), analyser(0), gridviewResults(0), modelCom(0), modelSpk(0)
    {}

    CorpusRepository *repository;
    AnalyserTemporal *analyser;
    GridViewWidget *gridviewResults;
    QStandardItemModel *modelCom;
    QStandardItemModel *modelSpk;
};

AnalyserTemporalWidget::AnalyserTemporalWidget(CorpusRepository *repository, AnalyserTemporal *analyser, QWidget *parent) :
    QWidget(parent), ui(new Ui::AnalyserTemporalWidget), d(new AnalyserTemporalWidgetData)
{
    ui->setupUi(this);
    if (!repository) return;
    d->repository = repository;
    // Analyser
    d->analyser = analyser;
    connect(d->analyser, SIGNAL(madeProgress(int)), this, SLOT(madeProgress(int)));
    // ================================================================================================================
    // MAIN WIDGET AND RESUTLS TAB
    // ================================================================================================================
    // Corpora combobox
    ui->comboBoxCorpus->addItems(repository->listCorporaIDs());
    // Command Analyse
    connect(ui->commandAnalyse, SIGNAL(clicked(bool)), this, SLOT(analyse()));
    // Results grid view
    d->gridviewResults = new GridViewWidget(this);
    d->gridviewResults->tableView()->verticalHeader()->setDefaultSectionSize(20);
    ui->gridLayoutResults->addWidget(d->gridviewResults);
    // Default
    ui->optionCommunications->setChecked(true);
    // Change display
    connect(ui->optionCommunications, SIGNAL(toggled(bool)), this, SLOT(changeDisplayedModel()));
    connect(ui->optionSpeakers, SIGNAL(toggled(bool)), this, SLOT(changeDisplayedModel()));
    // ================================================================================================================
    // BOXPLOTS TAB
    // ================================================================================================================
    // Measure combobox (start with measures for Communications by default)
    foreach (QString measureID, AnalyserTemporalItem::measureIDsForCommunication())
        ui->comboBoxMeasure->addItem(AnalyserTemporalItem::measureDefinition(measureID).displayName(), measureID);
    // Group by attributes
    if (d->repository->metadataStructure()) {
        ui->comboBoxGroupByCom->addItem("", "");
        ui->comboBoxGroupByCom->addItem("Communication ID", "ID");
        foreach (QPointer<MetadataStructureAttribute> attr, d->repository->metadataStructure()->attributes(CorpusObject::Type_Communication)) {
            ui->comboBoxGroupByCom->addItem(attr->name(), attr->ID());
        }
        ui->comboBoxGroupByCom->setCurrentText("");
        // speaker
        ui->comboBoxGroupBySpk->addItem("", "");
        ui->comboBoxGroupBySpk->addItem("Speaker ID", "ID");
        foreach (QPointer<MetadataStructureAttribute> attr, d->repository->metadataStructure()->attributes(CorpusObject::Type_Speaker)) {
            ui->comboBoxGroupBySpk->addItem(attr->name(), attr->ID());
        }
        ui->comboBoxGroupBySpk->setCurrentText("");
    }
    // Command Draw Chart
    connect(ui->commandDrawChart, SIGNAL(clicked(bool)), this, SLOT(drawBoxplot()));
    // Go to results tab
    ui->tabWidget->setCurrentIndex(0);
}

AnalyserTemporalWidget::~AnalyserTemporalWidget()
{
    delete ui;
    delete d;
}

void AnalyserTemporalWidget::madeProgress(int value)
{
    ui->progressBar->setValue(value);
    QApplication::processEvents();
}

void AnalyserTemporalWidget::analyse()
{
    if (!d->repository) return;
    QString corpusID = ui->comboBoxCorpus->currentText();

    // Analyse corpus
    QPointer<Corpus> corpus = d->repository->metadata()->getCorpus(corpusID);
    if (!corpus) return;
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(corpus->communicationsCount());
    d->analyser->setCorpus(corpus);
    d->analyser->analyse();
    ui->progressBar->setValue(ui->progressBar->maximum());

    changeDisplayedModel();
}

void AnalyserTemporalWidget::changeDisplayedModel()
{
    if (ui->optionCommunications->isChecked()) {
        ui->comboBoxMeasure->clear();
        foreach (QString measureID, AnalyserTemporalItem::measureIDsForCommunication())
            ui->comboBoxMeasure->addItem(AnalyserTemporalItem::measureDefinition(measureID).displayName(), measureID);
        showAnalysisForCom();
    }  else {
        ui->comboBoxMeasure->clear();
        foreach (QString measureID, AnalyserTemporalItem::measureIDsForSpeaker())
            ui->comboBoxMeasure->addItem(AnalyserTemporalItem::measureDefinition(measureID).displayName(), measureID);
        showAnalysisForSpk();
    }
}

void AnalyserTemporalWidget::showAnalysisForCom()
{
    if (!d->repository) return;
    if (!d->analyser->corpus()) return;
    if (!d->modelCom) buildModelForCom();
    // Update table
    d->gridviewResults->tableView()->setModel(d->modelCom);
}

void AnalyserTemporalWidget::showAnalysisForSpk()
{
    if (!d->repository) return;
    if (!d->analyser->corpus()) return;
    if (!d->modelSpk) buildModelForSpk();
    // Update table
    d->gridviewResults->tableView()->setModel(d->modelSpk);
}

void AnalyserTemporalWidget::buildModelForCom()
{
    if (d->modelCom) delete d->modelCom;
    d->modelCom = new QStandardItemModel(this);
    if (!d->analyser->corpus()) return;
    // Create model headers
    QStringList labels;
    labels << "CommunicationID";
    foreach (QPointer<MetadataStructureAttribute> attr, d->repository->metadataStructure()->attributes(CorpusObject::Type_Communication))
        labels << attr->ID();
    foreach (QString measureID, AnalyserTemporalItem::measureIDsForCommunication()) labels << measureID;
    d->modelCom->setHorizontalHeaderLabels(labels);
    // Data
    foreach (QPointer<CorpusCommunication> com, d->analyser->corpus()->communications()) {
        if (!com) continue;
        if (!d->analyser->item(com->ID())) continue;

        QList<QStandardItem *> itemsCom;
        QStandardItem *item;
        item = new QStandardItem(); item->setData(com->ID(), Qt::DisplayRole); itemsCom << item;
        // properties
        foreach (QPointer<MetadataStructureAttribute> attr, d->repository->metadataStructure()->attributes(CorpusObject::Type_Communication)) {
            item = new QStandardItem(); item->setData(com->property(attr->ID()), Qt::DisplayRole); itemsCom << item;
        }
        // measures
        foreach (QString measureID, AnalyserTemporalItem::measureIDsForCommunication()) {
            // analyser->measureDefinitionForCommunication(measureID).displayNameUnit()
            item = new QStandardItem(); item->setData(d->analyser->item(com->ID())->measureCom(measureID), Qt::DisplayRole); itemsCom << item;
        }
        d->modelCom->appendRow(itemsCom);
    }
}

void AnalyserTemporalWidget::buildModelForSpk()
{
    if (d->modelSpk) delete d->modelSpk;
    d->modelSpk = new QStandardItemModel(this);
    if (!d->analyser->corpus()) return;
    // Create model headers
    QStringList labels;
    labels << "CommunicationID" << "SpeakerID";
    foreach (QPointer<MetadataStructureAttribute> attr, d->repository->metadataStructure()->attributes(CorpusObject::Type_Communication)) {
        labels << attr->ID();
    }
    foreach (QPointer<MetadataStructureAttribute> attr, d->repository->metadataStructure()->attributes(CorpusObject::Type_Speaker)) {
        labels << attr->ID();
    }
    foreach (QString measureID, AnalyserTemporalItem::measureIDsForSpeaker()) labels << measureID;
    d->modelSpk->setHorizontalHeaderLabels(labels);
    foreach (QPointer<CorpusCommunication> com, d->analyser->corpus()->communications()) {
        if (!com) continue;
        if (!d->analyser->item(com->ID())) continue;
        foreach (QString speakerID, d->analyser->item(com->ID())->speakerIDs()) {
            QList<QStandardItem *> itemsSpk;
            QStandardItem *item;
            item = new QStandardItem(); item->setData(com->ID(), Qt::DisplayRole); itemsSpk << item;
            item = new QStandardItem(); item->setData(speakerID, Qt::DisplayRole); itemsSpk << item;
            // properties
            foreach (QPointer<MetadataStructureAttribute> attr, d->repository->metadataStructure()->attributes(CorpusObject::Type_Communication)) {
                item = new QStandardItem(); item->setData(com->property(attr->ID()), Qt::DisplayRole); itemsSpk << item;
            }
            foreach (QPointer<MetadataStructureAttribute> attr, d->repository->metadataStructure()->attributes(CorpusObject::Type_Speaker)) {
                item = new QStandardItem(); item->setData(com->property(attr->ID()), Qt::DisplayRole); itemsSpk << item;
            }
            // measures
            foreach (QString measureID, AnalyserTemporalItem::measureIDsForSpeaker()) {
                // analyser->measureDefinitionForSpeaker(measureID).displayNameUnit();
                item = new QStandardItem(); item->setData(d->analyser->item(com->ID())->measureSpk(speakerID, measureID), Qt::DisplayRole); itemsSpk << item;
            }
            d->modelSpk->appendRow(itemsSpk);
        }
    }
}

void AnalyserTemporalWidget::drawBoxplot()
{
    if (!d->analyser) return;
    if (!d->analyser->corpus()) return;

    // Get parameters from user interface
    QString measureID = ui->comboBoxMeasure->currentData().toString();
    QStringList groupAttributeIDsCom; groupAttributeIDsCom << ui->comboBoxGroupByCom->currentData().toString();
    QStringList groupAttributeIDsSpk; groupAttributeIDsSpk << ui->comboBoxGroupBySpk->currentData().toString();
    // Aggregate selected measure, over selected metadata attributes
    QMap<QString, QList<double> > aggregates;
    QString groupAttributes;
    if (ui->optionCommunications->isChecked()) {
        aggregates = d->analyser->aggregateMeasureCom(measureID, groupAttributeIDsCom);
        groupAttributes = ui->comboBoxGroupByCom->currentText();
    }
    else {
        aggregates = d->analyser->aggregateMeasureSpk(measureID, groupAttributeIDsCom, groupAttributeIDsSpk);
        QStringList sl; sl << ui->comboBoxGroupByCom->currentText() << ui->comboBoxGroupBySpk->currentText();
        groupAttributes = sl.join(", ");
    }
    if (groupAttributes.endsWith(", ")) groupAttributes.chop(2);
    // Calculate boxplot series
    QBoxPlotSeries *series = new QBoxPlotSeries();
    series->setName(measureID);
    double min(0), max(0);
    foreach (QString groupID, aggregates.keys()) {
        QBoxSet *set = new QBoxSet(groupID);
        StatisticalSummary summary;
        summary.calculate(aggregates.value(groupID));
//        set->setValue(QBoxSet::LowerExtreme, summary.firstQuartile() - 1.5 * summary.interQuartileRange());
//        set->setValue(QBoxSet::UpperExtreme, summary.thirdQuartile() + 1.5 * summary.interQuartileRange());
        set->setValue(QBoxSet::LowerExtreme, summary.min());
        set->setValue(QBoxSet::UpperExtreme, summary.max());
        set->setValue(QBoxSet::Median, summary.median());
        set->setValue(QBoxSet::LowerQuartile, summary.firstQuartile());
        set->setValue(QBoxSet::UpperQuartile, summary.thirdQuartile());
        series->append(set);
        if (min > summary.min()) min = summary.min();
        if (max < summary.max()) max = summary.max();
    }
    // Create boxplot chart
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(QString("%1 per %2").arg(ui->comboBoxMeasure->currentText()).arg(groupAttributes));
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->createDefaultAxes();
    chart->axisY()->setMin(qRound(min * 0.9));
    chart->axisY()->setMax(qRound(max * 1.1));
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    // Clear previous chart
    QLayoutItem *item;
    while ((item = ui->gridLayoutChart->takeAt(0)) != 0) { delete item; }
    QList<QChartView *> chartviews;
    chartviews = findChildren<QChartView *>();
    qDeleteAll(chartviews);
    // Create new chart view widget
    QChartView *chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);
    // Show chart view
    ui->gridLayoutChart->addWidget(chartView);
}

} // namespace StatisticsPluginTemporal
} // namespace Plugins
} // namespace Praaline