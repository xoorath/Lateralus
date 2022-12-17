export module Lateralus.Platform.Platform.Null;

import Lateralus.Platform.Error;
import Lateralus.Platform.Platform;
import <optional>;

using namespace std;

namespace Lateralus::Platform::Null
{
export class Platform : public iPlatform
{
public:
    optional<Error> Init() override { return Error("Null::Platform"); }
};

} // namespace Lateralus::Platform::Null