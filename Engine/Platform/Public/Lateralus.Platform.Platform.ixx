export module Lateralus.Platform.Platform;

import <optional>;

import Lateralus.Core;
import Lateralus.Platform.Error;

using namespace std;

namespace Lateralus::Platform
{
export class iPlatform
{
public:
    virtual ~iPlatform() = default;
    virtual optional<Error> Init() = 0;
};
} // namespace Lateralus::Platform
