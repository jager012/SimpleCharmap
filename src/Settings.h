#pragma once

#define DeclareSetting(key) static const QString key = #key

namespace Settings {
    DeclareSetting(AlwaysOnTop);
    DeclareSetting(TableCellSize);
    DeclareSetting(DoubleClickAction);
    DeclareSetting(RememberSelectedGroups);
    DeclareSetting(SelectedGroups);
    DeclareSetting(RememberSelectedFont);
    DeclareSetting(Font);
}  // namespace Settings
