#include "Backend.hpp"

#include <iostream>
#include <QtConcurrent/QtConcurrent>

Backend::Backend(QObject *parent) : QObject(parent) {
}

void Backend::appendOutput(const QString &text) {
    m_output += text.toHtmlEscaped().replace("\n", "<br>");
    emit outputUpdated(m_output);
}

void Backend::printSuccess(const QString &text) {
    m_output += colorizeText(text, "#28a745");
    emit outputUpdated(m_output);
}

void Backend::printWarning(const QString &text) {
    m_output += colorizeText(text, "#ffc107");
    emit outputUpdated(m_output);
}

void Backend::printError(const QString &text) {
    m_output += colorizeText(text, "#dc3545");
    emit outputUpdated(m_output);
}

void Backend::printInfo(const QString &text) {
    m_output += colorizeText(text, "#0a00bf");
    emit outputUpdated(m_output);
}

void Backend::clearOutput() {
    m_output.clear();
    emit outputUpdated(m_output);
}


void Backend::build(const QString &source_code) {
    if (m_app_state == AppState::Building) return;
    emit editorLockChanged(m_lock_editor = true);
    emit buildingEnabledChanged(m_building_enabled = false);
    clearOutput();

    m_vm.reset();

    QtConcurrent::run([this, str = source_code.toStdString()] {
                asm_parsing::ParsedInstVec parsed_insts{};
                auto result = asm_parsing::parse_and_resolve(str, parsed_insts, m_vm.get_memory_layout().data_base);
                return std::make_pair(result, std::move(parsed_insts));
            })
            .then(this, [this](std::pair<int, asm_parsing::ParsedInstVec> result_pair) {
                auto &[result, insts] = result_pair;
                if (result != 0) {
                    printError("Parse error\n");
                    emit editorLockChanged(m_lock_editor = false);
                    emit buildingEnabledChanged(m_building_enabled = true);
                    return;
                }
                m_vm.load_program(insts);
                printSuccess("Build successful\n");
                emit editorLockChanged(m_lock_editor = false);
            });
}

void Backend::reset() {
    if (m_app_state == AppState::Building) return;
    emit editorLockChanged(m_lock_editor = false);
    emit buildingEnabledChanged(m_building_enabled = true);
    std::clog << "reset" << std::endl;
}

void Backend::setAppState(AppState state) {
    switch (state) {
        case AppState::Idle: {
            emit buildingEnabledChanged(m_building_enabled = true);
            emit editorLockChanged(m_lock_editor = false);
            break;
        }
        default: {
            emit buildingEnabledChanged(m_building_enabled = false);
            emit editorLockChanged(m_lock_editor = true);
            break;
        }
    }
}

QString Backend::colorizeText(const QString &text, const QString &color_code) {
    return QString("<span style='color: %1;'>%2</span>")
            .arg(color_code, text.toHtmlEscaped()
                .replace("\n", "<br>"));
}
