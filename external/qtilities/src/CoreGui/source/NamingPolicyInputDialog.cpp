/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of Qtilities.
**
** For licensing information, please see
** http://jpnaude.github.io/Qtilities/page_licensing.html
**
****************************************************************************/

#include <QVariant>

#include "NamingPolicyInputDialog.h"
#include "ui_NamingPolicyInputDialog.h"
#include "QtilitiesCoreGuiConstants.h"

#include <QtilitiesProperty>
#include <QtilitiesCoreConstants>
#include <Observer>

using namespace Qtilities::CoreGui::Constants;
using namespace Qtilities::CoreGui::Icons;
using namespace Qtilities::Core::Properties;

Qtilities::CoreGui::NamingPolicyInputDialog::NamingPolicyInputDialog(QWidget *parent) :
    INamingPolicyDialog(parent),
    ui(new Ui::NamingPolicyInputDialog)
{
    ui->setupUi(this);
    observer_context = tr("Context not set");

    connect(ui->btnGenerateNewName,SIGNAL(clicked()),SLOT(handleGeneratedNewNameButton()));
    connect(ui->txtDifferentName,SIGNAL(textChanged(QString)),SLOT(handleDifferentNameChange(QString)));
    connect(ui->radioAutoRename,SIGNAL(clicked()),SLOT(updateStatusMessage()));
    connect(ui->radioDifferentName,SIGNAL(clicked()),SLOT(updateStatusMessage()));
    connect(ui->radioReplaceConflicting,SIGNAL(clicked()),SLOT(updateStatusMessage()));
    connect(ui->radioReject,SIGNAL(clicked()),SLOT(updateStatusMessage()));

    // Init icons on buttons
    ui->btnDifferentNameValidityIndicator->setIcon(QIcon(qti_icon_ERROR_16x16));
    ui->btnGenerateNewName->setIcon(QIcon(qti_icon_REFRESH_16x16));
    ui->radioAutoRename->setChecked(true);

    setAttribute(Qt::WA_QuitOnClose,false);
}

Qtilities::CoreGui::NamingPolicyInputDialog::~NamingPolicyInputDialog()
{
    delete ui;
}

void Qtilities::CoreGui::NamingPolicyInputDialog::setContext(int context_id, const QString& context, const QIcon& window_icon) {
    observer_context = context;
    observer_id = context_id;
    if (!window_icon.isNull())
        setWindowIcon(window_icon);
}

void Qtilities::CoreGui::NamingPolicyInputDialog::accept() {
    Q_ASSERT(object != 0);

    if (ui->radioDifferentName->isChecked()) {
        if (subject_filter->evaluateName(ui->txtDifferentName->text()) == NamingPolicyFilter::Acceptable) {
            subject_filter->setName(object,ui->txtDifferentName->text());
            QDialog::accept();
        }
    } else if (ui->radioAutoRename->isChecked()) {
        subject_filter->setName(object,ui->txtAutoGeneratedName->text());
        QDialog::accept();
    } else if (ui->radioReplaceConflicting->isChecked()) {
        // Set the conflicting object in the subject filter
        subject_filter->setConflictingObject(conflicting_object);
        QDialog::accept();
    } else if (ui->radioReject->isChecked()) {
        QDialog::accept();
    }
}

void Qtilities::CoreGui::NamingPolicyInputDialog::reject() {
    QDialog::reject();
}

void Qtilities::CoreGui::NamingPolicyInputDialog::endValidationCycle() {
    ui->chkAllInThisCycle->setChecked(false);
}

bool Qtilities::CoreGui::NamingPolicyInputDialog::useCycleResolution() const {
    if (ui->chkAllInThisCycle->isChecked())
        return true;
    else
        return false;
}

void Qtilities::CoreGui::NamingPolicyInputDialog::setObject(QObject* obj) {
    if (!obj)
        return;

    object = obj;
}

QString Qtilities::CoreGui::NamingPolicyInputDialog::getName() {
    if (!object)
        return QString();

    if (subject_filter->isObjectNameManager(object)) {
        // We use the qti_prop_NAME property:
        QVariant object_name_prop;
        object_name_prop = object->property(qti_prop_NAME);
        if (object_name_prop.isValid() && object_name_prop.canConvert<SharedProperty>())
            return (object_name_prop.value<SharedProperty>()).value().toString();
    } else {
        // We use the qti_prop_ALIAS_MAP property:
        QVariant instance_names_prop;
        instance_names_prop = object->property(qti_prop_ALIAS_MAP);
        if (instance_names_prop.isValid() && instance_names_prop.canConvert<MultiContextProperty>())
            return (instance_names_prop.value<MultiContextProperty>()).value(observer_id).toString();

    }
    return QString();
}

bool Qtilities::CoreGui::NamingPolicyInputDialog::initialize(NamingPolicyFilter::NameValidity validity_result) {
    // Check if everything was specified in order to initialize this dialog.
    if (!object)
        return false;

    if (!subject_filter)
        return false;

    // OK, everything is specified.
    QString original_name = getName();
    if (!original_name.isEmpty())
        ui->lblOriginalName->setText(original_name);
    else
        ui->lblOriginalName->setText(tr("[Empty Name]"));

    ui->lblContext->setText(observer_context);
    ui->txtDifferentName->setText(original_name);
    ui->txtDifferentName->setValidator(subject_filter->getValidator());
    QString valid_name = subject_filter->generateValidName(original_name);
    if (valid_name.isEmpty()) {
        ui->txtAutoGeneratedName->setText(QString(tr("Unable to generate a valid name.")));
        ui->radioAutoRename->setEnabled(false);
    } else {
        ui->txtAutoGeneratedName->setText(subject_filter->generateValidName(original_name));
        ui->radioAutoRename->setEnabled(true);
    }
    updateStatusMessage();

    if (validity_result & NamingPolicyFilter::Invalid) {
        // Don't show the Replace option when name is invalid.
        ui->radioReplaceConflicting->setVisible(false);
    } else if (validity_result & NamingPolicyFilter::Duplicate) {
        // We only show the Replace option to users when the conflicting object is only attached to the
        // observer context which the naming policy filter uses (thus has only the one parent).
        conflicting_object = subject_filter->getConflictingObject(getName());
        if (conflicting_object) {
            int parent_count = Observer::parentCount(conflicting_object);
            if (parent_count > 1)
                ui->radioReplaceConflicting->setVisible(false);
            else {
                // Is the parent this context
                if (subject_filter->observer->contains(conflicting_object)) {
                    ui->radioReplaceConflicting->setVisible(true);
                } else
                    ui->radioReplaceConflicting->setVisible(false);
            }
        } else {
            ui->radioReplaceConflicting->setVisible(false);
        }
    }

    // Only display the "All in this cycle" check box when an validation cycle is active in the naming filter
    if (subject_filter->isValidationCycleActive())
        ui->chkAllInThisCycle->setVisible(true);
    else
        ui->chkAllInThisCycle->setVisible(false);

    return true;
}

Qtilities::CoreGui::NamingPolicyFilter::ResolutionPolicy Qtilities::CoreGui::NamingPolicyInputDialog::selectedResolution() const {
    if (ui->radioAutoRename->isChecked()) {
        return NamingPolicyFilter::AutoRename;
    } else if (ui->radioDifferentName->isChecked()) {
        return NamingPolicyFilter::PromptUser;
    } else if (ui->radioReplaceConflicting->isChecked()) {
        return NamingPolicyFilter::Replace;
    } else if (ui->radioReject->isChecked()) {
        return NamingPolicyFilter::Reject;
    }

    // If we get here, just return something to supress compiler warnings
    return NamingPolicyFilter::AutoRename;
}

QString Qtilities::CoreGui::NamingPolicyInputDialog::autoGeneratedName() const {
    return ui->txtAutoGeneratedName->text();
}

void Qtilities::CoreGui::NamingPolicyInputDialog::handleGeneratedNewNameButton() {
    if (!subject_filter)
        return;

     ui->txtAutoGeneratedName->setText(subject_filter->generateValidName(ui->txtAutoGeneratedName->text(),true));
}

void Qtilities::CoreGui::NamingPolicyInputDialog::handleDifferentNameChange(QString new_text) {
    Q_UNUSED(new_text)

    NamingPolicyFilter::NameValidity validity_result = subject_filter->evaluateName(ui->txtDifferentName->text());

    // Update different name icon
    if (validity_result == NamingPolicyFilter::Acceptable)
        ui->btnDifferentNameValidityIndicator->setIcon(QIcon(qti_icon_SUCCESS_16x16));
    else
        ui->btnDifferentNameValidityIndicator->setIcon(QIcon(qti_icon_ERROR_16x16));

    // Update problem description
    if ((validity_result & NamingPolicyFilter::Duplicate) && (validity_result & NamingPolicyFilter::Invalid)) {
        ui->lblProblemDescription->setText("<font color='red'>Name is invalid and not unique.</font>");
    } else if (validity_result & NamingPolicyFilter::Duplicate) {
        ui->lblProblemDescription->setText("<font color='red'>Unique names are required within this context.</font>");
    } else if (validity_result & NamingPolicyFilter::Invalid) {
        QValidator* validator = subject_filter->getValidator();
        QRegExpValidator* reg_exp_validator = qobject_cast<QRegExpValidator*> (validator);
        if (reg_exp_validator)
            ui->lblProblemDescription->setText(QString("<font color='red'>Name is not valid within this context.<br>Validation Expression: %1</font>").arg(reg_exp_validator->regExp().pattern()));
        else
            ui->lblProblemDescription->setText("<font color='red'>Name is not valid within this context.</font>");
    } else {
        ui->lblProblemDescription->setText("<font color='green'>New name is valid.</font>");
    }
}

void Qtilities::CoreGui::NamingPolicyInputDialog::updateStatusMessage() {
    if (ui->radioDifferentName->isChecked()) {
        handleDifferentNameChange(ui->txtDifferentName->text());
    } else if (ui->radioAutoRename->isChecked()) {
        ui->lblProblemDescription->setText("<font color='green'>Auto-generated name is valid.</font>");
    } else if (sender() == ui->radioReplaceConflicting) {
        ui->lblProblemDescription->setText("<font color='green'>Conflicting object will be replaced.</font>");
    } else if (ui->radioReject->isChecked()) {
        ui->lblProblemDescription->setText("<font color='green'>Operation will be canceled.</font>");
    }
}

void Qtilities::CoreGui::NamingPolicyInputDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
