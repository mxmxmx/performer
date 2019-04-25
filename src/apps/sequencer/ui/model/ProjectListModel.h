#pragma once

#include "Config.h"

#include "RoutableListModel.h"

#include "model/Project.h"

class ProjectListModel : public RoutableListModel {
public:
    ProjectListModel(Project &project) :
        _project(project)
    {}

    virtual int rows() const override {
        return Last;
    }

    virtual int columns() const override {
        return 2;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            formatName(Item(row), str);
        } else if (column == 1) {
            formatValue(Item(row), str);
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            editValue(Item(row), value, shift);
        }
    }

    virtual Routing::Target routingTarget(int row) const override {
        switch (Item(row)) {
        case Tempo:
            return Routing::Target::Tempo;
        case Swing:
            return Routing::Target::Swing;
        default:
            return Routing::Target::None;
        }
    }

private:
    enum Item {
        Name,
        Tempo,
        Swing,
        SyncMeasure,
        Scale,
        RootNote,
        RecordMode,
        CvGateInput,
        Last
    };

    static const char *itemName(Item item) {
        switch (item) {
        case Name:              return "Name";
        case Tempo:             return "Tempo";
        case Swing:             return "Swing";
        case SyncMeasure:       return "Sync Measure";
        case Scale:             return "Scale";
        case RootNote:          return "Root Note";
        case RecordMode:        return "Record Mode";
        case CvGateInput:       return "CV/Gate Input";
        case Last:              break;
        }
        return nullptr;
    }

    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case Name:
            str(_project.name());
            break;
        case Tempo:
            _project.printTempo(str);
            break;
        case Swing:
            _project.printSwing(str);
            break;
        case SyncMeasure:
            _project.printSyncMeasure(str);
            break;
        case Scale:
            _project.printScale(str);
            break;
        case RootNote:
            _project.printRootNote(str);
            break;
        case RecordMode:
            _project.printRecordMode(str);
            break;
        case CvGateInput:
            _project.printCvGateInput(str);
            break;
        case Last:
            break;
        }
    }

    void editValue(Item item, int value, bool shift) {
        switch (item) {
        case Name:
            break;
        case Tempo:
            _project.editTempo(value, shift);
            break;
        case Swing:
            _project.editSwing(value, shift);
            break;
        case SyncMeasure:
            _project.editSyncMeasure(value, shift);
            break;
        case Scale:
            _project.editScale(value, shift);
            break;
        case RootNote:
            _project.editRootNote(value, shift);
            break;
        case RecordMode:
            _project.editRecordMode(value, shift);
            break;
        case CvGateInput:
            _project.editCvGateInput(value, shift);
            break;
        case Last:
            break;
        }
    }

    Project &_project;
};
