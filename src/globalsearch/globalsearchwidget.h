/* This file is part of Clementine.
   Copyright 2010, David Sansome <me@davidsansome.com>

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GLOBALSEARCHWIDGET_H
#define GLOBALSEARCHWIDGET_H

#include "searchprovider.h"

#include <QScopedPointer>
#include <QWidget>

class GlobalSearch;
class GlobalSearchTooltip;
class LibraryBackendInterface;
class Ui_GlobalSearchWidget;

class QDesktopWidget;
class QListView;
class QMimeData;
class QModelIndex;
class QSortFilterProxyModel;
class QStandardItemModel;
class QToolButton;


class GlobalSearchWidget : public QWidget {
  Q_OBJECT

public:
  GlobalSearchWidget(QWidget* parent = 0);
  ~GlobalSearchWidget();

  static const int kMinVisibleItems;
  static const int kMaxVisibleItems;
  static const int kSwapModelsTimeoutMsec;

  enum Role {
    Role_PrimaryResult = Qt::UserRole + 1,
    Role_AllResults,
    Role_LazyLoadingArt,
    Role_OrderArrived
  };

  void Init(GlobalSearch* engine_);

  // Called by the delegate
  void LazyLoadArt(const QModelIndex& index);

  // QWidget
  bool eventFilter(QObject* o, QEvent* e);

public slots:
  void ReloadSettings();

signals:
  void AddToPlaylist(QMimeData* data);

protected:
  void resizeEvent(QResizeEvent* e);
  void paintEvent(QPaintEvent* e);

private slots:
  void TextEdited(const QString& text);
  void AddResults(int id, const SearchProvider::ResultList& results);

  void ArtLoaded(int id, const QPixmap& pixmap);

  void TracksLoaded(int id, MimeData* mime_data);

  void ResultDoubleClicked();
  void AddCurrent();
  void AddAndPlayCurrent();
  void AddAndQueueCurrent();
  void ReplaceCurrent();
  void ReplaceAndPlayCurrent();

  void HidePopup();
  void UpdateTooltip();

  void SwapModels();

  void ProviderAdded(const SearchProvider* provider);
  void ProviderRemoved(const SearchProvider* provider);
  void ProviderButtonToggled(bool on);

private:
  // Return values from CanCombineResults
  enum CombineAction {
    CannotCombine,  // The two results are different and can't be combined
    LeftPreferred,  // The two results can be combined - the left one is better
    RightPreferred  // The two results can be combined - the right one is better
  };

  void RepositionPopup();
  CombineAction CanCombineResults(const QModelIndex& left, const QModelIndex& right) const;
  void CombineResults(const QModelIndex& superior, const QModelIndex& inferior);

  bool EventFilterSearchWidget(QObject* o, QEvent* e);
  bool EventFilterPopup(QObject* o, QEvent* e);
  bool EventFilterProviderButton(QToolButton* button, QEvent* e);

  void LoadTracks(QAction* trigger);

private:
  Ui_GlobalSearchWidget* ui_;

  GlobalSearch* engine_;
  int last_id_;
  int order_arrived_counter_;

  QMap<int, QModelIndex> art_requests_;
  QMap<int, QAction*> track_requests_;

  // Like graphics APIs have a front buffer and a back buffer, there's a front
  // model and a back model - the front model is the one that's shown in the
  // UI and the back model is the one that lies in wait.  current_model_ will
  // point to either the front or the back model.
  QStandardItemModel* front_model_;
  QStandardItemModel* back_model_;
  QStandardItemModel* current_model_;

  QSortFilterProxyModel* front_proxy_;
  QSortFilterProxyModel* back_proxy_;
  QSortFilterProxyModel* current_proxy_;

  QListView* view_;
  bool consume_focus_out_;

  QTimer* swap_models_timer_;

  QPixmap background_;
  QPixmap background_scaled_;

  QDesktopWidget* desktop_;

  bool combine_identical_results_;
  QStringList provider_order_;

  QScopedPointer<GlobalSearchTooltip> tooltip_;

  QAction* add_;
  QAction* add_and_play_;
  QAction* add_and_queue_;
  QAction* replace_;
  QAction* replace_and_play_;
  QList<QAction*> actions_;

  QMap<const SearchProvider*, QToolButton*> provider_buttons_;
};

#endif // GLOBALSEARCHWIDGET_H
