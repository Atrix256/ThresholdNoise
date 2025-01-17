#pragma once

#include "technique.h"

namespace Threshold
{
    inline PyObject* NoiseTypesToString(PyObject* self, PyObject* args)
    {
        int value;
        if (!PyArg_ParseTuple(args, "i:NoiseTypesToString", &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        switch((NoiseTypes)value)
        {
            case NoiseTypes::White: return Py_BuildValue("s", "White");
            case NoiseTypes::Blue2D_Offset: return Py_BuildValue("s", "Blue2D_Offset");
            case NoiseTypes::Blue2D_Flipbook: return Py_BuildValue("s", "Blue2D_Flipbook");
            case NoiseTypes::Blue2D_Golden_Ratio: return Py_BuildValue("s", "Blue2D_Golden_Ratio");
            case NoiseTypes::STBN_10: return Py_BuildValue("s", "STBN_10");
            case NoiseTypes::STBN_19: return Py_BuildValue("s", "STBN_19");
            case NoiseTypes::FAST_Blue_Exp_Separate: return Py_BuildValue("s", "FAST_Blue_Exp_Separate");
            case NoiseTypes::FAST_Blue_Exp_Product: return Py_BuildValue("s", "FAST_Blue_Exp_Product");
            case NoiseTypes::FAST_Binomial3x3_Exp: return Py_BuildValue("s", "FAST_Binomial3x3_Exp");
            case NoiseTypes::FAST_Box3x3_Exp: return Py_BuildValue("s", "FAST_Box3x3_Exp");
            case NoiseTypes::Blue_Tellusim_128_128_64: return Py_BuildValue("s", "Blue_Tellusim_128_128_64");
            case NoiseTypes::Blue_Stable_Fiddusion: return Py_BuildValue("s", "Blue_Stable_Fiddusion");
            case NoiseTypes::R2: return Py_BuildValue("s", "R2");
            case NoiseTypes::IGN: return Py_BuildValue("s", "IGN");
            case NoiseTypes::Bayer4: return Py_BuildValue("s", "Bayer4");
            case NoiseTypes::Bayer16: return Py_BuildValue("s", "Bayer16");
            case NoiseTypes::Bayer64: return Py_BuildValue("s", "Bayer64");
            case NoiseTypes::Bayer256: return Py_BuildValue("s", "Bayer256");
            case NoiseTypes::Round: return Py_BuildValue("s", "Round");
            case NoiseTypes::Floor: return Py_BuildValue("s", "Floor");
            default: return Py_BuildValue("s", "<invalid NoiseTypes value>");
        }
    }

    inline PyObject* SpatialFiltersToString(PyObject* self, PyObject* args)
    {
        int value;
        if (!PyArg_ParseTuple(args, "i:SpatialFiltersToString", &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        switch((SpatialFilters)value)
        {
            case SpatialFilters::None: return Py_BuildValue("s", "None");
            case SpatialFilters::Box: return Py_BuildValue("s", "Box");
            case SpatialFilters::Gauss: return Py_BuildValue("s", "Gauss");
            default: return Py_BuildValue("s", "<invalid SpatialFilters value>");
        }
    }

    inline PyObject* Set_Threshold(PyObject* self, PyObject* args)
    {
        int contextIndex;
        float value;

        if (!PyArg_ParseTuple(args, "if:Set_Threshold", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_Threshold = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_BrightnessMultiplier(PyObject* self, PyObject* args)
    {
        int contextIndex;
        float value;

        if (!PyArg_ParseTuple(args, "if:Set_BrightnessMultiplier", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_BrightnessMultiplier = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_Auto_Brightness(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_Auto_Brightness", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_Auto_Brightness = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_Animate(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_Animate", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_Animate = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_NoiseType1(PyObject* self, PyObject* args)
    {
        int contextIndex;
        int value;

        if (!PyArg_ParseTuple(args, "ii:Set_NoiseType1", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_NoiseType1 = (NoiseTypes)value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_SpatialFilter1(PyObject* self, PyObject* args)
    {
        int contextIndex;
        int value;

        if (!PyArg_ParseTuple(args, "ii:Set_SpatialFilter1", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_SpatialFilter1 = (SpatialFilters)value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_SpatialFilterParam1(PyObject* self, PyObject* args)
    {
        int contextIndex;
        float value;

        if (!PyArg_ParseTuple(args, "if:Set_SpatialFilterParam1", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_SpatialFilterParam1 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_TemporalFilterAlpha1(PyObject* self, PyObject* args)
    {
        int contextIndex;
        float value;

        if (!PyArg_ParseTuple(args, "if:Set_TemporalFilterAlpha1", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_TemporalFilterAlpha1 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_NeighborhoodClamp1(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_NeighborhoodClamp1", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_NeighborhoodClamp1 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_Animate1(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_Animate1", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_Animate1 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_NoiseType2(PyObject* self, PyObject* args)
    {
        int contextIndex;
        int value;

        if (!PyArg_ParseTuple(args, "ii:Set_NoiseType2", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_NoiseType2 = (NoiseTypes)value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_SpatialFilter2(PyObject* self, PyObject* args)
    {
        int contextIndex;
        int value;

        if (!PyArg_ParseTuple(args, "ii:Set_SpatialFilter2", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_SpatialFilter2 = (SpatialFilters)value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_SpatialFilterParam2(PyObject* self, PyObject* args)
    {
        int contextIndex;
        float value;

        if (!PyArg_ParseTuple(args, "if:Set_SpatialFilterParam2", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_SpatialFilterParam2 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_TemporalFilterAlpha2(PyObject* self, PyObject* args)
    {
        int contextIndex;
        float value;

        if (!PyArg_ParseTuple(args, "if:Set_TemporalFilterAlpha2", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_TemporalFilterAlpha2 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_NeighborhoodClamp2(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_NeighborhoodClamp2", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_NeighborhoodClamp2 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_Animate2(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_Animate2", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_Animate2 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_NoiseType3(PyObject* self, PyObject* args)
    {
        int contextIndex;
        int value;

        if (!PyArg_ParseTuple(args, "ii:Set_NoiseType3", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_NoiseType3 = (NoiseTypes)value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_SpatialFilter3(PyObject* self, PyObject* args)
    {
        int contextIndex;
        int value;

        if (!PyArg_ParseTuple(args, "ii:Set_SpatialFilter3", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_SpatialFilter3 = (SpatialFilters)value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_SpatialFilterParam3(PyObject* self, PyObject* args)
    {
        int contextIndex;
        float value;

        if (!PyArg_ParseTuple(args, "if:Set_SpatialFilterParam3", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_SpatialFilterParam3 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_TemporalFilterAlpha3(PyObject* self, PyObject* args)
    {
        int contextIndex;
        float value;

        if (!PyArg_ParseTuple(args, "if:Set_TemporalFilterAlpha3", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_TemporalFilterAlpha3 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_NeighborhoodClamp3(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_NeighborhoodClamp3", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_NeighborhoodClamp3 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_Animate3(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_Animate3", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_Animate3 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_NoiseType4(PyObject* self, PyObject* args)
    {
        int contextIndex;
        int value;

        if (!PyArg_ParseTuple(args, "ii:Set_NoiseType4", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_NoiseType4 = (NoiseTypes)value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_SpatialFilter4(PyObject* self, PyObject* args)
    {
        int contextIndex;
        int value;

        if (!PyArg_ParseTuple(args, "ii:Set_SpatialFilter4", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_SpatialFilter4 = (SpatialFilters)value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_SpatialFilterParam4(PyObject* self, PyObject* args)
    {
        int contextIndex;
        float value;

        if (!PyArg_ParseTuple(args, "if:Set_SpatialFilterParam4", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_SpatialFilterParam4 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_TemporalFilterAlpha4(PyObject* self, PyObject* args)
    {
        int contextIndex;
        float value;

        if (!PyArg_ParseTuple(args, "if:Set_TemporalFilterAlpha4", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_TemporalFilterAlpha4 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_NeighborhoodClamp4(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_NeighborhoodClamp4", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_NeighborhoodClamp4 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_Animate4(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_Animate4", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_Animate4 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    static PyMethodDef pythonModuleMethods[] = {
        {"NoiseTypesToString", NoiseTypesToString, METH_VARARGS, ""},
        {"SpatialFiltersToString", SpatialFiltersToString, METH_VARARGS, ""},
        {"Set_Threshold", Set_Threshold, METH_VARARGS, ""},
        {"Set_BrightnessMultiplier", Set_BrightnessMultiplier, METH_VARARGS, "Stippling can dim the result by adding dark pixels. This can brighten it."},
        {"Set_Auto_Brightness", Set_Auto_Brightness, METH_VARARGS, "if true, sets BrightnessMultiplier to 1/Threshold"},
        {"Set_Animate", Set_Animate, METH_VARARGS, ""},
        {"Set_NoiseType1", Set_NoiseType1, METH_VARARGS, "Upper Left"},
        {"Set_SpatialFilter1", Set_SpatialFilter1, METH_VARARGS, ""},
        {"Set_SpatialFilterParam1", Set_SpatialFilterParam1, METH_VARARGS, "Radius for box, sigma for gauss"},
        {"Set_TemporalFilterAlpha1", Set_TemporalFilterAlpha1, METH_VARARGS, "Alpha for exponential moving average. 0.1 is common for TAA."},
        {"Set_NeighborhoodClamp1", Set_NeighborhoodClamp1, METH_VARARGS, ""},
        {"Set_Animate1", Set_Animate1, METH_VARARGS, "If false, does not animate, even if the global Animate variable is true"},
        {"Set_NoiseType2", Set_NoiseType2, METH_VARARGS, "Upper Right"},
        {"Set_SpatialFilter2", Set_SpatialFilter2, METH_VARARGS, ""},
        {"Set_SpatialFilterParam2", Set_SpatialFilterParam2, METH_VARARGS, "Radius for box, sigma for gauss"},
        {"Set_TemporalFilterAlpha2", Set_TemporalFilterAlpha2, METH_VARARGS, "Alpha for exponential moving average. 0.1 is common for TAA."},
        {"Set_NeighborhoodClamp2", Set_NeighborhoodClamp2, METH_VARARGS, ""},
        {"Set_Animate2", Set_Animate2, METH_VARARGS, "If false, does not animate, even if the global Animate variable is true"},
        {"Set_NoiseType3", Set_NoiseType3, METH_VARARGS, "Lower Left"},
        {"Set_SpatialFilter3", Set_SpatialFilter3, METH_VARARGS, ""},
        {"Set_SpatialFilterParam3", Set_SpatialFilterParam3, METH_VARARGS, "Radius for box, sigma for gauss"},
        {"Set_TemporalFilterAlpha3", Set_TemporalFilterAlpha3, METH_VARARGS, "Alpha for exponential moving average. 0.1 is common for TAA."},
        {"Set_NeighborhoodClamp3", Set_NeighborhoodClamp3, METH_VARARGS, ""},
        {"Set_Animate3", Set_Animate3, METH_VARARGS, "If false, does not animate, even if the global Animate variable is true"},
        {"Set_NoiseType4", Set_NoiseType4, METH_VARARGS, "Lower Right"},
        {"Set_SpatialFilter4", Set_SpatialFilter4, METH_VARARGS, ""},
        {"Set_SpatialFilterParam4", Set_SpatialFilterParam4, METH_VARARGS, "Radius for box, sigma for gauss"},
        {"Set_TemporalFilterAlpha4", Set_TemporalFilterAlpha4, METH_VARARGS, "Alpha for exponential moving average. 0.1 is common for TAA."},
        {"Set_NeighborhoodClamp4", Set_NeighborhoodClamp4, METH_VARARGS, ""},
        {"Set_Animate4", Set_Animate4, METH_VARARGS, "If false, does not animate, even if the global Animate variable is true"},
        {nullptr, nullptr, 0, nullptr}
    };

    static PyModuleDef pythonModule = {
        PyModuleDef_HEAD_INIT, "Threshold", NULL, -1, pythonModuleMethods,
        NULL, NULL, NULL, NULL
    };

    PyObject* CreateModule()
    {
        PyObject* module = PyModule_Create(&pythonModule);
        PyModule_AddIntConstant(module, "NoiseTypes_White", 0);
        PyModule_AddIntConstant(module, "NoiseTypes_Blue2D_Offset", 1);
        PyModule_AddIntConstant(module, "NoiseTypes_Blue2D_Flipbook", 2);
        PyModule_AddIntConstant(module, "NoiseTypes_Blue2D_Golden_Ratio", 3);
        PyModule_AddIntConstant(module, "NoiseTypes_STBN_10", 4);
        PyModule_AddIntConstant(module, "NoiseTypes_STBN_19", 5);
        PyModule_AddIntConstant(module, "NoiseTypes_FAST_Blue_Exp_Separate", 6);
        PyModule_AddIntConstant(module, "NoiseTypes_FAST_Blue_Exp_Product", 7);
        PyModule_AddIntConstant(module, "NoiseTypes_FAST_Binomial3x3_Exp", 8);
        PyModule_AddIntConstant(module, "NoiseTypes_FAST_Box3x3_Exp", 9);
        PyModule_AddIntConstant(module, "NoiseTypes_Blue_Tellusim_128_128_64", 10);
        PyModule_AddIntConstant(module, "NoiseTypes_Blue_Stable_Fiddusion", 11);
        PyModule_AddIntConstant(module, "NoiseTypes_R2", 12);
        PyModule_AddIntConstant(module, "NoiseTypes_IGN", 13);
        PyModule_AddIntConstant(module, "NoiseTypes_Bayer4", 14);
        PyModule_AddIntConstant(module, "NoiseTypes_Bayer16", 15);
        PyModule_AddIntConstant(module, "NoiseTypes_Bayer64", 16);
        PyModule_AddIntConstant(module, "NoiseTypes_Bayer256", 17);
        PyModule_AddIntConstant(module, "NoiseTypes_Round", 18);
        PyModule_AddIntConstant(module, "NoiseTypes_Floor", 19);
        PyModule_AddIntConstant(module, "SpatialFilters_None", 0);
        PyModule_AddIntConstant(module, "SpatialFilters_Box", 1);
        PyModule_AddIntConstant(module, "SpatialFilters_Gauss", 2);
        return module;
    }
};
