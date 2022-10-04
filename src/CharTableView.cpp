#include "CharTableView.h"

#include <QHeaderView>
#include <QSettings>

#include "CharTableModel.h"

CharTableView::CharTableView(QWidget* parent)
    : QTableView(parent), m_cellSize(CharTableView::CellSizeRegular) {
    qRegisterMetaType<CharTableView::CellSize>();
}

void CharTableView::setModel(QAbstractItemModel* m) {
    if (CharTableModel* oldModel = dynamic_cast<CharTableModel*>(model())) {
        disconnect(this, &CharTableView::cellSizeChanged, oldModel, &CharTableModel::setCellSize);
        disconnect(this, &CharTableView::widthChanged, oldModel, &CharTableModel::setWidth);
        disconnect(oldModel, &CharTableModel::selectionUpdated, this,
                   &CharTableView::resizeRowsToContents);
    }

    if (CharTableModel* newModel = dynamic_cast<CharTableModel*>(m)) {
        newModel->setWidth(width());
        newModel->setCellSize(m_cellSize.cell);
        connect(newModel, &CharTableModel::selectionUpdated, this,
            &CharTableView::resizeRowsToContents);
        connect(this, &CharTableView::widthChanged, newModel, &CharTableModel::setWidth);
        connect(this, &CharTableView::cellSizeChanged, newModel, &CharTableModel::setCellSize);
    }

    QTableView::setModel(m);
}

void CharTableView::resizeEvent(QResizeEvent* event) {
    emit widthChanged(event->size().width());

    resizeColumnsToContents();
    resizeRowsToContents();
}

void CharTableView::setCellSize(CellSize size) {
    if (m_cellSize != size) {
        m_cellSize = size;

        QFont f = font();
        f.setPointSize(size.font);
        setFont(f);

        emit cellSizeChanged(m_cellSize.cell);

        resizeColumnsToContents();
        resizeRowsToContents();
    }
}

// disables font merging and stuff
// use when changing font with the combobox
void CharTableView::changeFont(const QFont& font) {
    QFont f = font;
    f.setStyleStrategy(QFont::NoFontMerging);
    f.setPointSize(m_cellSize.font);
    setFont(f);
    emit fontChanged(f);
}

QDataStream& operator<<(QDataStream& out, const CharTableView::CellSize& v) {
    return out << v.font << v.cell;
}

QDataStream& operator>>(QDataStream& in, CharTableView::CellSize& v) {
    return in >> v.font >> v.cell;
}

bool operator==(const CharTableView::CellSize& a, const CharTableView::CellSize& b) {
    return a.font == b.font && a.cell == b.cell;
}

bool operator!=(const CharTableView::CellSize& a, const CharTableView::CellSize& b) {
    return a.font != b.font || a.cell != b.cell;
}
