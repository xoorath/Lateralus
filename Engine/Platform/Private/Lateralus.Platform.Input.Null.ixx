export module Lateralus.Platform.Input.Null;

import Lateralus.Platform.Error;
import Lateralus.Platform.Input;
import <optional>;

using namespace std;

namespace Lateralus::Platform::Input::Null
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