#ifndef __SGE_ASSET_MANAGER_HPP
#define __SGE_ASSET_MANAGER_HPP

#include <sge/assets/locator.hpp>
#include <sge/assets/loader.hpp>
#include <sge/assets/asset.hpp>
#include <sge/assets/cache.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace sge
{
    class AssetManager
    {
        private:
            std::vector<std::shared_ptr<AssetLocator>> locators;
            std::unordered_map<std::string, std::shared_ptr<AssetLoader>> loaders;

            AssetCache cache;

        public:
            void register_locator(std::shared_ptr<AssetLocator> locator);
            void register_loader(std::shared_ptr<AssetLoader> loader, std::vector<std::string> const &extensions);

            void unload(BaseAsset *asset);

            template <typename A, typename D>
            A *load(D const &assetdesc)
            {
                static_assert(std::is_base_of<BaseAsset, A>::value, "Supplied asset type does not inherit from Asset");
                static_assert(std::is_base_of<AssetDescriptor, D>::value, "Supplied descriptor type does not inherit from AssetDescriptor");

                A *asset = nullptr;

                if (cache.find(assetdesc) == cache.end())
                {
                    SDL_RWops *input = nullptr;

                    for (auto it = locators.begin(); it != locators.end(); it++)
                    {
                        auto locator = *it;

                        try
                        {
                            input = locator->locate(assetdesc.name());
                        }
                        catch (AssetLocatorError const &e)
                        {
                            std::cerr << "[AssetLocatorError] " << e.what() << std::endl;
                            input = nullptr;
                        }

                        if (input != nullptr)
                        {
                            break;
                        }
                    }

                    if (input != nullptr)
                    {
                        if (loaders.find(assetdesc.extension()) != loaders.end())
                        {
                            auto loader = loaders[assetdesc.extension()];
                            asset = new A(assetdesc);

                            try
                            {
                                loader->load(asset, input);
                            }
                            catch (AssetLoaderError const &e)
                            {
                                std::cerr << "[AssetLoaderError] " << e.what() << std::endl;
                                delete asset;
                                asset = nullptr;
                            }

                            if (asset != nullptr)
                            {
                                cache[assetdesc] = asset;
                            }
                        }
                        else
                        {
                            std::cerr << "[AssetLoaderError] No loader found for extension: " << assetdesc.extension() << std::endl;
                            delete asset;
                            asset = nullptr;
                        }
                    }
                }
                else
                {
                    asset = cache[assetdesc];
                }

                if (asset != nullptr)
                {
                    asset->acquire();
                }

                return asset;
            }
    };
}

#endif /* __SGE_ASSET_MANAGER_HPP */
