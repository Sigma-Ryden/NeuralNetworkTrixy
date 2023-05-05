#include <iostream> // cout

#include <Auto/TestingCore.hpp>

TestingInterface::TestingInterface(const char* module, const char* name)
    : module(module), name(name)
{
    TestingCore::instance().add(this);
}

TestingCore::TestingCore()
{
    text_printer = console_printer();
}

TestingCore& TestingCore::instance()
{
    static TestingCore self;
    return self;
}

static std::string info_format(TestingInterface* test, const char* msg, bool contition)
{
    return std::string(test->module)
         + "::"
         + test->name
         + '.'
         + msg
         + " - "
         + (contition?"Ok":"Fail")
         + '\n';
}

static void update_stat(unsigned& passed, unsigned& failed, bool ok)
{
    passed += ok;
    failed += not ok;
}

void TestingCore::check(bool condition, TestingInterface* test, const char* msg)
{
    update_stat(passed, failed, condition);
    text_printer(info_format(test, msg, condition));
}

void TestingCore::add(TestingInterface* test)
{
    registry_[test->module][test->name] = test;
}

void TestingCore::execute_module(const char* name)
{
    auto it = registry_.find(name);
    if (it == registry_.end()) return;

    auto& module = it->second;
    for (auto& name_test : module) name_test.second->run();
}

void TestingCore::execute_test(const char* name)
{
    for (auto& name_module : registry_)
    {
        auto& module = name_module.second;

        auto it = module.find(name);
        if (it == module.end()) continue;

        it->second->run();
    }
}

void TestingCore::execute_all()
{
    for (auto& name_module : registry_)
    {
        auto& module = name_module.second;
        for (auto& name_test : module) name_test.second->run();
    }
}

static std::string stat_format(unsigned passed, unsigned failed)
{
    return "\nPassed: "
         + std::to_string(passed)
         + " Failed: "
         + std::to_string(failed)
         + " Total: "
         + std::to_string(passed+failed)
         + '\n';
}

void TestingCore::stat()
{
    text_printer(stat_format(passed, failed));
}

void TestingCore::try_catch(std::function<void()>&& call) const noexcept
{
    try { call(); }
    catch(const char* e) { text_printer(e); }
    catch(std::exception& e) { text_printer(e.what()); }
    catch(...) { text_printer("Unexpected error."); }
}

auto TestingCore::console_printer() const noexcept -> TextPrinter
{
    return [](const std::string& str) { std::cout << str; };
}
