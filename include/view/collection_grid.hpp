#pragma once

#include <borealis.hpp>
#include "view/recycling_grid.hpp"
#include "state/image_state.hpp"

#include <borealis/core/event.hpp>

namespace collection {

  struct CollectionItem {
    std::string file = "";
    Image image;
    bool selected = false;
  };

  class RecyclerCell
      : public RecyclingGridItem
  {
  public:
    RecyclerCell();

    BRLS_BIND(brls::Image, image, "image");

    virtual void prepareForReuse() override
    {
      image->clear();
      img.clear();
    }

    virtual void cacheForReuse() override
    {
      image->clear();
      img.clear();
    }

    virtual void onFocusGained() override;

    static RecyclerCell *create(std::function<void(std::string)> cb);
    CollectionItem* ref;
    std::string img = "";
    std::function<void(std::string)> cb;
  };

  class DataSource
      : public RecyclingGridDataSource
  {
  public:
    DataSource(std::vector<CollectionItem> items, std::function<void(std::string)> onSelected, brls::View *parent);
    bool onItemAction(RecyclingGrid *recycler, size_t index, brls::ControllerButton button) override;

    RecyclingGridItem *cellForRow(RecyclingGrid *recycler, size_t index) override;

    void onItemSelected(RecyclingGrid *recycler, size_t index) override;

    void clearData() override;

    size_t getItemCount() override;

    void deleteSelected();

    void updateCell(RecyclingGridItem* item, size_t index) override;

    std::function<void(std::string)> onSelected;
    std::vector<CollectionItem> items;
    brls::View *parent = nullptr;
  };

  class CollectionGrid : public brls::Box
  {
  public:
    CollectionGrid(const std::vector<std::string> &files, std::string title, ImageState &state, std::function<void(std::string)> onSelected, std::function<void(std::string, ImageState &state)> onFocused);

  private:
    BRLS_BIND(RecyclingGrid, recycler, "recycler");
    BRLS_BIND(brls::Image, workingImage, "image");
    BRLS_BIND(brls::Button, confirmDelete, "confirm_delete");
  };

}
