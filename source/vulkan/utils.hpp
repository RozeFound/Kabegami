#pragma once

#include <optional>

namespace vku {

    struct QueueFamilyIndices {
        
        std::optional<uint32_t> transfer_family;
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;
        std::optional<uint32_t> compute_family;

        constexpr bool is_complete ( ) {
            if (transfer_family.has_value()
                && graphics_family.has_value()
                && present_family.has_value()
                && compute_family.has_value())
                return true;
            return false;
        }
        
	};

    constexpr uint32_t to_u32 (std::size_t value) { return static_cast<uint32_t>(value); }

}