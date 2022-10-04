#pragma once

#include <QActionGroup>
#include <QMainWindow>

#include "CharTableModel.h"
#include "GroupsTreeModel.h"
#include "Settings.h"
#include "UnicodeGroups.h"
#include "UnicodePoints.h"
#include "ui_MainWindow.h"
#include "ui_LicenseDialog.h"

class MainWindow : public QMainWindow {
        Q_OBJECT

    public:
        enum DoubleClickAction {
            CopyCell = 0,
            CopyText = 1,
            NoOp = 2,
        };

        MainWindow(QWidget* parent = Q_NULLPTR);

        void closeEvent(QCloseEvent* event) override;

        UnicodePoints getCodePoints() const { return m_codePoints; };

    private:
        Ui::MainWindow ui;
        Ui::LicenseDialog licenseUi;

        QDialog m_licenseDialog;

        UnicodePoints m_codePoints;
        UnicodeGroups m_groupsTree;

        CharTableModel m_charTableModel;
        GroupsTreeModel m_groupsTreeModel;

        DoubleClickAction m_dblClickAction;

        QActionGroup m_sizeActionGroup;
        QActionGroup m_copyActionGroup;

        bool _loadData();
        void _loadUiSettings();
        void _setupMenu();

    public slots:
        void about();

        void alwaysOnTop(bool b);

        void charTableAction(const QModelIndex& index);
        void copyText();
};
