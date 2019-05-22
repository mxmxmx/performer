#pragma once

#include "Config.h"

#include "Serialize.h"

#include "core/utils/StringBuilder.h"
#include "core/math/Math.h"

#include <array>

/* use grades A, B rather than C, D for full 16 bit range */
#define GRADE_AB 
/* adjusted Vbias because didn't have the resistor values (as per BOM) on hand ... see defaultItemValue() below */
#define OFFSET_0995 

#ifdef GRADE_AB
 #define MAX_VALUE 0x10000
#else
 #define MAX_VALUE 0x8000
#endif

class Calibration {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    class CvOutput {
    public:
        static constexpr int MinVoltage = -5;
        static constexpr int MaxVoltage = 5;
        static constexpr int ItemsPerVolt = 1;
        static constexpr int ItemCount = (MaxVoltage - MinVoltage) * ItemsPerVolt + 1;

        typedef std::array<uint32_t, ItemCount> ItemArray;

        // items

        static float itemToVolts(int index) {
            return float(index) / ItemsPerVolt + MinVoltage;
        }

        static void itemName(StringBuilder &str, int index) {
            str("%+.1fV", itemToVolts(index));
        }

        const ItemArray &items() const { return _items; }
              ItemArray &items()       { return _items; }

        int item(int index) const {
            return _items[index] & (MAX_VALUE - 0x1);
        }

        void setItem(int index, int value, bool doUpdate = true) {
            _items[index] = (_items[index] & MAX_VALUE) | clamp(value, 0, (MAX_VALUE - 0x1));
            if (doUpdate) {
                update();
            }
        }

        void editItem(int index, int value, int shift) {
            // inverted to improve usability
            setItem(index, item(index) - value * (shift ? 50 : 1));
        }

        void printItem(int index, StringBuilder &str) const {
            // inverted to improve usability
            if (userDefined(index)) {
                str("%d", (MAX_VALUE - 0x1) - item(index));
            } else {
                str("%d (auto)", (MAX_VALUE - 0x1) - item(index));
            }
        }

        bool userDefined(int index) const {
            return _items[index] & MAX_VALUE;
        }

        void setUserDefined(int index, bool value) {
            _items[index] = (_items[index] & (MAX_VALUE - 0x1)) | (value ? MAX_VALUE : 0);
            update();
        }

        int defaultItemValue(int index) const {
            // In ideal DAC/OpAmp configuration we get:
            // 0     ->  5.17V
            // 32768 -> -5.25V
            #ifdef OFFSET_0995
            // used 49k9/33k --> Vbias = 0.995V
             static constexpr float volts0 = 5.14f;
             static constexpr float volts1 = -5.27f;
            #else
             static constexpr float volts0 = 5.17f;
             static constexpr float volts1 = -5.25f;
            #endif

            float volts = itemToVolts(index);

            return clamp(int((volts - volts0) / (volts1 - volts0) * MAX_VALUE), 0, (MAX_VALUE - 0x1));
        }

        uint16_t voltsToValue(float volts) const {
            volts = clamp(volts, float(MinVoltage), float(MaxVoltage));
            float fIndex = (volts - MinVoltage) * ItemsPerVolt;
            int index = std::floor(fIndex);
            if (index < ItemCount - 1) {
                float t = fIndex - index;
                return lerp(t, item(index), item(index + 1));
            } else {
                return item(ItemCount - 1);
            }
        }

        void clear();

        void write(WriteContext &context) const;
        void read(ReadContext &context);

    private:
        void update();

        ItemArray _items;
    };

    typedef std::array<CvOutput, CONFIG_CV_OUTPUT_CHANNELS> CvOutputArray;

    //----------------------------------------
    // cvOutputs
    //----------------------------------------

    const CvOutputArray &cvOutputs() const { return _cvOutputs; }
          CvOutputArray &cvOutputs()       { return _cvOutputs; }

    const CvOutput &cvOutput(int index) const { return _cvOutputs[index]; }
          CvOutput &cvOutput(int index)       { return _cvOutputs[index]; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    void clear();

    void write(WriteContext &context) const;
    void read(ReadContext &context);

private:
    CvOutputArray _cvOutputs;
};
