#include "annotationmodewidget.h"
#include "ui_annotationmodewidget.h"

#include "automaticannotationwidget.h"
#include "manualannotationwidget.h"
#include "batcheditorwidget.h"
#include "compareannotationswidget.h"

struct AnnotationModeWidgetData {
    AnnotationModeWidgetData()
    { }

    QAction *actionShowManualAnnotation;
    QAction *actionShowAutomaticAnnotation;
    QAction *actionShowBatchEditor;
    QAction *actionShowCompareAnnotations;

    AutomaticAnnotationWidget *widgetAutomaticAnnotation;
    ManualAnnotationWidget *widgetManualAnnotation;
    BatchEditorWidget *widgetBatchEditor;
    CompareAnnotationsWidget *widgetCompareAnnotations;
};


AnnotationModeWidget::AnnotationModeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AnnotationModeWidget), d(new AnnotationModeWidgetData)
{
    ui->setupUi(this);

    d->widgetAutomaticAnnotation = new AutomaticAnnotationWidget(this);
    d->widgetManualAnnotation = new ManualAnnotationWidget(this);
    d->widgetBatchEditor = new BatchEditorWidget(this);
    d->widgetCompareAnnotations = new CompareAnnotationsWidget(this);

    ui->gridLayoutAutomaticAnnotation->addWidget(d->widgetAutomaticAnnotation);
    ui->gridLayoutManualAnnotation->addWidget(d->widgetManualAnnotation);
    ui->gridLayoutBatchEditor->addWidget(d->widgetBatchEditor);
    ui->gridLayoutCompareAnnotations->addWidget(d->widgetCompareAnnotations);

    setupActions();

    connect(ui->commandAutomatic, SIGNAL(clicked()), this, SLOT(showAutomaticAnnotation()));
    connect(ui->commandTimelineEditor, SIGNAL(clicked()), this, SLOT(showManualAnnotation()));
    connect(ui->commandBatchEditor, SIGNAL(clicked()), this, SLOT(showBatchEditor()));
    connect(ui->commandCompareAnnotations, SIGNAL(clicked()), this, SLOT(showCompareAnnotations()));

    ui->stackedWidget->setCurrentIndex(1);
}

AnnotationModeWidget::~AnnotationModeWidget()
{
    delete ui;
    if (d->widgetAutomaticAnnotation) delete d->widgetAutomaticAnnotation;
    if (d->widgetManualAnnotation) delete d->widgetManualAnnotation;
    if (d->widgetBatchEditor) delete d->widgetBatchEditor;
    if (d->widgetCompareAnnotations) delete d->widgetCompareAnnotations;
    delete d;
}

void AnnotationModeWidget::setupActions()
{
    // Context and menu bar
    QList<int> context;
    context.push_front(CONTEXT_MANAGER->contextID(qti_def_CONTEXT_STANDARD));

    bool existed;
    Command* command;

    ActionContainer* menu_bar = ACTION_MANAGER->menuBar(qti_action_MENUBAR_STANDARD);
    ActionContainer* view_menu = ACTION_MANAGER->createMenu(qti_action_VIEW, existed);
    if (!existed) menu_bar->addMenu(view_menu, qti_action_HELP);

    // ------------------------------------------------------------------------------------------------------
    // VIEW MENU
    // ------------------------------------------------------------------------------------------------------
    d->actionShowManualAnnotation = new QAction("Show Vertical Timeline Editor", this);
    connect(d->actionShowManualAnnotation, SIGNAL(triggered()), SLOT(showManualAnnotation()));
    command = ACTION_MANAGER->registerAction("Annotation.ShowManual", d->actionShowManualAnnotation, context);
    command->setCategory(QtilitiesCategory(QApplication::applicationName()));
    view_menu->addAction(command);

    d->actionShowAutomaticAnnotation = new QAction("Show Automatic Annotation", this);
    connect(d->actionShowAutomaticAnnotation, SIGNAL(triggered()), SLOT(showAutomaticAnnotation()));
    command = ACTION_MANAGER->registerAction("Annotation.ShowAutomatic", d->actionShowAutomaticAnnotation, context);
    command->setCategory(QtilitiesCategory(QApplication::applicationName()));
    view_menu->addAction(command);

    d->actionShowBatchEditor = new QAction("Show Batch Editor", this);
    connect(d->actionShowBatchEditor, SIGNAL(triggered()), SLOT(showBatchEditor()));
    command = ACTION_MANAGER->registerAction("Annotation.ShowBatch", d->actionShowBatchEditor, context);
    command->setCategory(QtilitiesCategory(QApplication::applicationName()));
    view_menu->addAction(command);

    d->actionShowCompareAnnotations = new QAction("Show Compare Annotations", this);
    connect(d->actionShowCompareAnnotations, SIGNAL(triggered()), SLOT(showCompareAnnotations()));
    command = ACTION_MANAGER->registerAction("Annotation.ShowCompareAnnotations", d->actionShowCompareAnnotations, context);
    command->setCategory(QtilitiesCategory(QApplication::applicationName()));
    view_menu->addAction(command);

    view_menu->addSeperator();
}

void AnnotationModeWidget::showManualAnnotation()
{
    ui->stackedWidget->setCurrentIndex(1);
    emit activateMode();
}

void AnnotationModeWidget::showAutomaticAnnotation()
{
    ui->stackedWidget->setCurrentIndex(0);
    emit activateMode();
}

void AnnotationModeWidget::showBatchEditor()
{
    ui->stackedWidget->setCurrentIndex(2);
    emit activateMode();
}

void AnnotationModeWidget::showCompareAnnotations()
{
    ui->stackedWidget->setCurrentIndex(3);
    emit activateMode();
}