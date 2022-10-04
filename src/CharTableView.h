#pragma once

#include <QResizeEvent>
#include <QTableView>

#include "CharTableModel.h"

class CharTableView : public QTableView {
        Q_OBJECT

    public:
        struct CellSize {
                int font;
                int cell;
        };

        inline static const CellSize CellSizeSmall = {10, 25};
        inline static const CellSize CellSizeRegular = {11, 35};
        inline static const CellSize CellSizeLarge = {16, 50};
        inline static const CellSize CellSizeExtraLarge = {20, 70};

        CharTableView(QWidget* parent = nullptr);

        void setModel(QAbstractItemModel* m);

        void setCellSize(CellSize size);
        void changeFont(const QFont& font);

    protected:
        void resizeEvent(QResizeEvent* event) override;

    signals:
        void cellSizeChanged(int size);
        void widthChanged(int w);
        void fontChanged(const QFont& f);

    private:
        CellSize m_cellSize;
};

QDataStream& operator<<(QDataStream& out, const CharTableView::CellSize& v);
QDataStream& operator>>(QDataStream& in, CharTableView::CellSize& v);
bool operator==(const CharTableView::CellSize& a, const CharTableView::CellSize& b);
bool operator!=(const CharTableView::CellSize& a, const CharTableView::CellSize& b);

Q_DECLARE_METATYPE(CharTableView::CellSize)
