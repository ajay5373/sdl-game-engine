#ifndef __SGE_FILE_LOCATOR_HPP
#define __SGE_FILE_LOCATOR_HPP

#include <sge/assets/locator.hpp>

namespace sge
{
    class FileLocator : public AssetLocator
    {
        public:
            FileLocator(std::string const &location);

            virtual SDL_RWops *locate(std::string const &assetname);

        private:
            std::string _location;
    };
}

#endif /* __SGE_FILE_LOCATOR_HPP */
