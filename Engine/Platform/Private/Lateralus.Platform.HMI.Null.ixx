export module Lateralus.Platform.HMI.Null;

import Lateralus.Platform.Error;
import Lateralus.Platform.HMI;
import <optional>;

using namespace std;

namespace Lateralus::Platform::HMI::Null
{
export class InputProvider : public iInputProvider
{
public:
    InputProvider() = default;
    ~InputProvider() = default;

    optional<Error> Init() override { return Error("Not implemented"); }

    void Shutdown() override {}
};
} // namespace Lateralus::Platform::Input::Null