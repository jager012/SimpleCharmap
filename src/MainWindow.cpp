#include "MainWindow.h"

#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QFontComboBox>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>

#include "CharTableModel.h"
#include "CharTableView.h"
#include "Settings.h"
#include "UnicodeGroups.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_licenseDialog(this),
      m_charTableModel(m_codePoints, m_groupsTree, this),
      m_groupsTreeModel(m_groupsTree, this),
      m_dblClickAction(DoubleClickAction::CopyCell),
      m_sizeActionGroup(this),
      m_copyActionGroup(this) {
    if (!_loadData()) {
        exit(-1);
    }

    ui.setupUi(this);
    licenseUi.setupUi(&m_licenseDialog);

    QList<int> splitterSizes(2);
    splitterSizes[0] = width() / 10 * 3;
    splitterSizes[1] = width() / 10 * 7;
    ui.splitter->setSizes(splitterSizes);

    _setupMenu();

    // reflect selection changes in table model
    connect(&m_groupsTreeModel, &GroupsTreeModel::selectionChanged, &m_charTableModel,
            &CharTableModel::setSelection);

    // same, but for fonts
    connect(ui.fontComboBox, &QFontComboBox::currentFontChanged, ui.tableView,
            &CharTableView::changeFont);
    connect(ui.tableView, &CharTableView::fontChanged, [this](const QFont& font) {
        QFont f = font;
        f.setPointSize(ui.textEdit->font().pointSize());
        ui.textEdit->setFont(f);
    });
    _loadUiSettings();

    // ui.tableView->changeFont(ui.tableView->font()); // disable font merging
    ui.tableView->setModel(&m_charTableModel);

    // display hint on status bar when navigating the table using keyboard arrows
    connect(ui.tableView->selectionModel(), &QItemSelectionModel::currentChanged,
            [this](const QModelIndex& index, const QModelIndex& _) {
        ui.statusBar->showMessage(m_charTableModel.statusHint(index));
    });

    ui.treeView->setUniformRowHeights(true);
    ui.treeView->setModel(&m_groupsTreeModel);

    connect(ui.groupFilter, &QLineEdit::textEdited, &m_groupsTreeModel,
            &GroupsTreeModel::filterItems);
    connect(ui.tableView, &QTableView::doubleClicked, this, &MainWindow::charTableAction);

    connect(ui.clearButton, &QPushButton::pressed, ui.textEdit, &QLineEdit::clear);
    connect(ui.copyButton, &QPushButton::pressed, this, &MainWindow::copyText);
}

bool MainWindow::_loadData() {
    QFile cp(QApplication::applicationDirPath() + "/codepoints.dat");
    if (!cp.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, qtTrId("criticalMsgTitle"), cp.errorString() + " (codepoints.dat)");
        return false;
    }

    QFile grps(QApplication::applicationDirPath() + "/groups.xml");
    if (!grps.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, qtTrId("criticalMsgTitle"), cp.errorString() + " (groups.xml)");
        return false;
    }

    if (!m_codePoints.load(&cp)) {
        QMessageBox::critical(this, qtTrId("criticalMsgTitle"),
                              qtTrId("criticalParsingError").arg("codepoints.dat"));
        return false;
    }

    if (!m_groupsTree.load(&grps)) {
        QMessageBox::critical(this, qtTrId("criticalMsgTitle"),
                              qtTrId("criticalParsingError").arg("groups.xml"));
        return false;
    }

    return true;
}

void MainWindow::_setupMenu() {
    ui.actionNoOp->setData(DoubleClickAction::NoOp);
    ui.actionCopyCell->setData(DoubleClickAction::CopyCell);
    ui.actionCopyText->setData(DoubleClickAction::CopyText);

    m_copyActionGroup.addAction(ui.actionNoOp);
    m_copyActionGroup.addAction(ui.actionCopyCell);
    m_copyActionGroup.addAction(ui.actionCopyText);
    m_copyActionGroup.setExclusive(true);

    ui.actionCellSmall->setData(
                QVariant::fromValue<CharTableView::CellSize>(CharTableView::CellSizeSmall));
    ui.actionCellRegular->setData(
                QVariant::fromValue<CharTableView::CellSize>(CharTableView::CellSizeRegular));
    ui.actionCellLarge->setData(
                QVariant::fromValue<CharTableView::CellSize>(CharTableView::CellSizeLarge));
    ui.actionCellXL->setData(
                QVariant::fromValue<CharTableView::CellSize>(CharTableView::CellSizeExtraLarge));

    m_sizeActionGroup.addAction(ui.actionCellSmall);
    m_sizeActionGroup.addAction(ui.actionCellRegular);
    m_sizeActionGroup.addAction(ui.actionCellLarge);
    m_sizeActionGroup.addAction(ui.actionCellXL);
    m_sizeActionGroup.setExclusive(true);

    connect(&m_sizeActionGroup, &QActionGroup::triggered, [this](QAction* action) {
        ui.tableView->setCellSize(action->data().value<CharTableView::CellSize>());
    });

    connect(&m_copyActionGroup, &QActionGroup::triggered, [this](QAction* action) {
        m_dblClickAction = action->data().value<DoubleClickAction>();
    });

    connect(ui.actionAlwaysOnTop, &QAction::triggered, this, &MainWindow::alwaysOnTop);

    connect(ui.actionLicense, &QAction::triggered, &m_licenseDialog, &QDialog::exec);
    connect(ui.actionAboutSimpleCharmap, &QAction::triggered, this, &MainWindow::about);
    connect(ui.actionAboutQt, &QAction::triggered,
            static_cast<QApplication*>(QApplication::instance()), &QApplication::aboutQt);
}

void MainWindow::_loadUiSettings() {
    QSettings settings;

    ui.actionRememberGroups->setChecked(
                settings.value(Settings::RememberSelectedGroups)
                .toBool());  // setting gets read on exit, visual update only

    bool alwaysOnTop = settings.value(Settings::AlwaysOnTop).toBool();
    if (alwaysOnTop) setWindowFlag(Qt::WindowStaysOnTopHint, alwaysOnTop);
    ui.actionAlwaysOnTop->setChecked(alwaysOnTop);

    CharTableView::CellSize tableCellSize =
            settings
            .value(Settings::TableCellSize,
                   QVariant::fromValue<CharTableView::CellSize>(CharTableView::CellSizeRegular))
            .value<CharTableView::CellSize>();
    if (tableCellSize == CharTableView::CellSizeSmall)
        ui.actionCellSmall->setChecked(true);
    else if (tableCellSize == CharTableView::CellSizeLarge)
        ui.actionCellLarge->setChecked(true);
    else if (tableCellSize == CharTableView::CellSizeExtraLarge)
        ui.actionCellXL->setChecked(true);
    else
        ui.actionCellRegular->setChecked(true);
    ui.tableView->setCellSize(tableCellSize);

    m_dblClickAction =
            static_cast<DoubleClickAction>(settings.value(Settings::DoubleClickAction).toInt());
    switch (m_dblClickAction) {
        case DoubleClickAction::CopyText:
            ui.actionCopyText->setChecked(true);
            break;
        case DoubleClickAction::NoOp:
            ui.actionNoOp->setChecked(true);
            break;
        default:
            ui.actionCopyCell->setChecked(true);
    }

    bool rememberFont = settings.value(Settings::RememberSelectedFont).toBool();
    ui.actionRememberFont->setChecked(rememberFont);
    if (rememberFont) {
        QFont font = settings.value(Settings::Font, ui.tableView->font()).value<QFont>();
        ui.fontComboBox->setCurrentFont(font);
    } else
        ui.tableView->changeFont(ui.tableView->font());   // disable font merging for default font

    if (settings.value(Settings::RememberSelectedGroups).toBool()) {
        int size = settings.beginReadArray(Settings::SelectedGroups);
        QList<quint32> selection;
        selection.reserve(size + 1);
        for (int i = 0; i < size; i++) {
            settings.setArrayIndex(i);
            selection.append(settings.value(Settings::SelectedGroups).toInt());
        }
        settings.endArray();
        m_groupsTreeModel.restoreSelection(selection);
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    QSettings settings;
    settings.setValue(Settings::AlwaysOnTop, ui.actionAlwaysOnTop->isChecked());
    settings.setValue(Settings::DoubleClickAction, m_dblClickAction);
    settings.setValue(Settings::TableCellSize, m_sizeActionGroup.checkedAction()->data());

    bool rememberFont = ui.actionRememberFont->isChecked();
    settings.setValue(Settings::RememberSelectedFont, rememberFont);
    if (rememberFont) settings.setValue(Settings::Font, ui.fontComboBox->currentFont());

    bool rememberGroups = ui.actionRememberGroups->isChecked();
    settings.setValue(Settings::RememberSelectedGroups, rememberGroups);
    if (!rememberGroups) return;

    QHash<TreeNode<UnicodeGroup>*, Qt::CheckState> selection = m_groupsTreeModel.selection();
    settings.beginWriteArray(Settings::SelectedGroups);
    int index = 0;
    for (auto it = selection.begin(); it != selection.end(); it++) {
        if (it.value() == Qt::Checked && it.key()->data().to != 0) {
            settings.setArrayIndex(index);
            settings.setValue(Settings::SelectedGroups, it.key()->data().from);
            index++;
        }
    }
    settings.endArray();
}

void MainWindow::about() {
    QMessageBox::about(this, qtTrId("aboutDlgTitle"), qtTrId("aboutDlgContent"));
}

void MainWindow::alwaysOnTop(bool b) {
    setWindowFlag(Qt::WindowStaysOnTopHint, b);
    show();
}

void MainWindow::charTableAction(const QModelIndex& index) {
    if (m_dblClickAction == DoubleClickAction::NoOp) return;

    quint32 cp = m_charTableModel.lookupIndex(index);
    QString cpString = QString::fromUcs4(reinterpret_cast<char32_t*>(&cp), 1);
    if (!ui.textEdit->text().contains(cpString)) ui.textEdit->setText(ui.textEdit->text() + cpString);

    if (m_dblClickAction == DoubleClickAction::CopyText) {
        QApplication::clipboard()->setText(ui.textEdit->text());
        ui.statusBar->showMessage(qtTrId("textCopiedStatusMsg"), 3000);
    } else {
        QApplication::clipboard()->setText(cpString);
        ui.statusBar->showMessage(qtTrId("characterCopiedStatusMsg").arg(cpString), 3000);
    }
}

void MainWindow::copyText() {
    if (ui.textEdit->hasSelectedText())
        ui.textEdit->copy();
    else
        QApplication::clipboard()->setText(ui.textEdit->text());

    ui.statusBar->showMessage(qtTrId("textCopiedStatusMsg"), 3000);
}
