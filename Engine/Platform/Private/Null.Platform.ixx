export module Lateralus.Platform.Null.Platform;

import Lateralus.Platform.Error;
import Lateralus.Platform.iPlatform;
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