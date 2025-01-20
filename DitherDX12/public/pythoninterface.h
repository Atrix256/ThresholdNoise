#pragma once

#include "technique.h"

namespace Dither
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
            case NoiseTypes::FAST_Triangle_Blue_Exp_Separate: return Py_BuildValue("s", "FAST_Triangle_Blue_Exp_Separate");
            case NoiseTypes::FAST_Triangle_Blue_Exp_Product: return Py_BuildValue("s", "FAST_Triangle_Blue_Exp_Product");
            case NoiseTypes::FAST_Binomial3x3_Exp: return Py_BuildValue("s", "FAST_Binomial3x3_Exp");
            case NoiseTypes::FAST_Box3x3_Exp: return Py_BuildValue("s", "FAST_Box3x3_Exp");
            case NoiseTypes::Blue_Tellusim_128_128_64: return Py_BuildValue("s", "Blue_Tellusim_128_128_64");
            case NoiseTypes::Blue_Stable_Fiddusion: return Py_BuildValue("s", "Blue_Stable_Fiddusion");
            case NoiseTypes::R2: return Py_BuildValue("s", "R2");
            case NoiseTypes::IGN: return Py_BuildValue("s", "IGN");
            case NoiseTypes::Bayer: return Py_BuildValue("s", "Bayer");
            case NoiseTypes::Bayer_Plus_Half: return Py_BuildValue("s", "Bayer_Plus_Half");
            case NoiseTypes::Round: return Py_BuildValue("s", "Round");
            case NoiseTypes::Floor: return Py_BuildValue("s", "Floor");
            case NoiseTypes::White4: return Py_BuildValue("s", "White4");
            case NoiseTypes::White4_Plus_Half: return Py_BuildValue("s", "White4_Plus_Half");
            case NoiseTypes::White8: return Py_BuildValue("s", "White8");
            case NoiseTypes::White8_Plus_Half: return Py_BuildValue("s", "White8_Plus_Half");
            case NoiseTypes::White512: return Py_BuildValue("s", "White512");
            case NoiseTypes::White_Triangular: return Py_BuildValue("s", "White_Triangular");
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

    inline PyObject* TemporalFiltersToString(PyObject* self, PyObject* args)
    {
        int value;
        if (!PyArg_ParseTuple(args, "i:TemporalFiltersToString", &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        switch((TemporalFilters)value)
        {
            case TemporalFilters::None: return Py_BuildValue("s", "None");
            case TemporalFilters::EMA: return Py_BuildValue("s", "EMA");
            case TemporalFilters::EMA_plus_Clamp: return Py_BuildValue("s", "EMA_plus_Clamp");
            case TemporalFilters::Monte_Carlo: return Py_BuildValue("s", "Monte_Carlo");
            default: return Py_BuildValue("s", "<invalid TemporalFilters value>");
        }
    }

    inline PyObject* Set_Reset_Accumulation(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_Reset_Accumulation", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_Reset_Accumulation = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_BitsPerColorChannel(PyObject* self, PyObject* args)
    {
        int contextIndex;
        int value;

        if (!PyArg_ParseTuple(args, "ii:Set_BitsPerColorChannel", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_BitsPerColorChannel = value;

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

    inline PyObject* Set_ExtendNoise1(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_ExtendNoise1", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_ExtendNoise1 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_SubtractiveDither1(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_SubtractiveDither1", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_SubtractiveDither1 = value;

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

    inline PyObject* Set_TemporalFilter1(PyObject* self, PyObject* args)
    {
        int contextIndex;
        int value;

        if (!PyArg_ParseTuple(args, "ii:Set_TemporalFilter1", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_TemporalFilter1 = (TemporalFilters)value;

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

    inline PyObject* Set_ExtendNoise2(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_ExtendNoise2", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_ExtendNoise2 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_SubtractiveDither2(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_SubtractiveDither2", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_SubtractiveDither2 = value;

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

    inline PyObject* Set_TemporalFilter2(PyObject* self, PyObject* args)
    {
        int contextIndex;
        int value;

        if (!PyArg_ParseTuple(args, "ii:Set_TemporalFilter2", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_TemporalFilter2 = (TemporalFilters)value;

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

    inline PyObject* Set_ExtendNoise3(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_ExtendNoise3", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_ExtendNoise3 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_SubtractiveDither3(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_SubtractiveDither3", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_SubtractiveDither3 = value;

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

    inline PyObject* Set_TemporalFilter3(PyObject* self, PyObject* args)
    {
        int contextIndex;
        int value;

        if (!PyArg_ParseTuple(args, "ii:Set_TemporalFilter3", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_TemporalFilter3 = (TemporalFilters)value;

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

    inline PyObject* Set_ExtendNoise4(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_ExtendNoise4", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_ExtendNoise4 = value;

        Py_INCREF(Py_None);
        return Py_None;
    }

    inline PyObject* Set_SubtractiveDither4(PyObject* self, PyObject* args)
    {
        int contextIndex;
        bool value;

        if (!PyArg_ParseTuple(args, "ib:Set_SubtractiveDither4", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_SubtractiveDither4 = value;

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

    inline PyObject* Set_TemporalFilter4(PyObject* self, PyObject* args)
    {
        int contextIndex;
        int value;

        if (!PyArg_ParseTuple(args, "ii:Set_TemporalFilter4", &contextIndex, &value))
            return PyErr_Format(PyExc_TypeError, "type error");

        Context* context = Context::GetContext(contextIndex);
        if (!context)
            return PyErr_Format(PyExc_IndexError, __FUNCTION__, "() : index % i is out of range(count = % i)", contextIndex, Context::GetContextCount());

        context->m_input.variable_TemporalFilter4 = (TemporalFilters)value;

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
        {"TemporalFiltersToString", TemporalFiltersToString, METH_VARARGS, ""},
        {"Set_Reset_Accumulation", Set_Reset_Accumulation, METH_VARARGS, ""},
        {"Set_BitsPerColorChannel", Set_BitsPerColorChannel, METH_VARARGS, ""},
        {"Set_Animate", Set_Animate, METH_VARARGS, ""},
        {"Set_NoiseType1", Set_NoiseType1, METH_VARARGS, "Upper Left"},
        {"Set_ExtendNoise1", Set_ExtendNoise1, METH_VARARGS, "If true, uses a 2d low discrepancy shuffle to offset the texture every cycle, using all offsets before repeating, in a low discrepancy pattern."},
        {"Set_SubtractiveDither1", Set_SubtractiveDither1, METH_VARARGS, "If true, simulates subtractive dither by dequantizing after quantization, and subtracting dither value."},
        {"Set_SpatialFilter1", Set_SpatialFilter1, METH_VARARGS, ""},
        {"Set_SpatialFilterParam1", Set_SpatialFilterParam1, METH_VARARGS, "Radius for box, sigma for gauss"},
        {"Set_TemporalFilter1", Set_TemporalFilter1, METH_VARARGS, ""},
        {"Set_TemporalFilterAlpha1", Set_TemporalFilterAlpha1, METH_VARARGS, "Alpha for exponential moving average. 0.1 is common for TAA."},
        {"Set_Animate1", Set_Animate1, METH_VARARGS, "If false, does not animate, even if the global Animate variable is true"},
        {"Set_NoiseType2", Set_NoiseType2, METH_VARARGS, "Upper Right"},
        {"Set_ExtendNoise2", Set_ExtendNoise2, METH_VARARGS, "If true, uses a 2d low discrepancy shuffle to offset the texture every cycle, using all offsets before repeating, in a low discrepancy pattern."},
        {"Set_SubtractiveDither2", Set_SubtractiveDither2, METH_VARARGS, "If true, simulates subtractive dither by dequantizing after quantization, and subtracting dither value."},
        {"Set_SpatialFilter2", Set_SpatialFilter2, METH_VARARGS, ""},
        {"Set_SpatialFilterParam2", Set_SpatialFilterParam2, METH_VARARGS, "Radius for box, sigma for gauss"},
        {"Set_TemporalFilter2", Set_TemporalFilter2, METH_VARARGS, ""},
        {"Set_TemporalFilterAlpha2", Set_TemporalFilterAlpha2, METH_VARARGS, "Alpha for exponential moving average. 0.1 is common for TAA."},
        {"Set_Animate2", Set_Animate2, METH_VARARGS, "If false, does not animate, even if the global Animate variable is true"},
        {"Set_NoiseType3", Set_NoiseType3, METH_VARARGS, "Lower Left"},
        {"Set_ExtendNoise3", Set_ExtendNoise3, METH_VARARGS, "If true, uses a 2d low discrepancy shuffle to offset the texture every cycle, using all offsets before repeating, in a low discrepancy pattern."},
        {"Set_SubtractiveDither3", Set_SubtractiveDither3, METH_VARARGS, "If true, simulates subtractive dither by dequantizing after quantization, and subtracting dither value."},
        {"Set_SpatialFilter3", Set_SpatialFilter3, METH_VARARGS, ""},
        {"Set_SpatialFilterParam3", Set_SpatialFilterParam3, METH_VARARGS, "Radius for box, sigma for gauss"},
        {"Set_TemporalFilter3", Set_TemporalFilter3, METH_VARARGS, ""},
        {"Set_TemporalFilterAlpha3", Set_TemporalFilterAlpha3, METH_VARARGS, "Alpha for exponential moving average. 0.1 is common for TAA."},
        {"Set_Animate3", Set_Animate3, METH_VARARGS, "If false, does not animate, even if the global Animate variable is true"},
        {"Set_NoiseType4", Set_NoiseType4, METH_VARARGS, "Lower Right"},
        {"Set_ExtendNoise4", Set_ExtendNoise4, METH_VARARGS, "If true, uses a 2d low discrepancy shuffle to offset the texture every cycle, using all offsets before repeating, in a low discrepancy pattern."},
        {"Set_SubtractiveDither4", Set_SubtractiveDither4, METH_VARARGS, "If true, simulates subtractive dither by dequantizing after quantization, and subtracting dither value."},
        {"Set_SpatialFilter4", Set_SpatialFilter4, METH_VARARGS, ""},
        {"Set_SpatialFilterParam4", Set_SpatialFilterParam4, METH_VARARGS, "Radius for box, sigma for gauss"},
        {"Set_TemporalFilter4", Set_TemporalFilter4, METH_VARARGS, ""},
        {"Set_TemporalFilterAlpha4", Set_TemporalFilterAlpha4, METH_VARARGS, "Alpha for exponential moving average. 0.1 is common for TAA."},
        {"Set_Animate4", Set_Animate4, METH_VARARGS, "If false, does not animate, even if the global Animate variable is true"},
        {nullptr, nullptr, 0, nullptr}
    };

    static PyModuleDef pythonModule = {
        PyModuleDef_HEAD_INIT, "Dither", NULL, -1, pythonModuleMethods,
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
        PyModule_AddIntConstant(module, "NoiseTypes_FAST_Triangle_Blue_Exp_Separate", 8);
        PyModule_AddIntConstant(module, "NoiseTypes_FAST_Triangle_Blue_Exp_Product", 9);
        PyModule_AddIntConstant(module, "NoiseTypes_FAST_Binomial3x3_Exp", 10);
        PyModule_AddIntConstant(module, "NoiseTypes_FAST_Box3x3_Exp", 11);
        PyModule_AddIntConstant(module, "NoiseTypes_Blue_Tellusim_128_128_64", 12);
        PyModule_AddIntConstant(module, "NoiseTypes_Blue_Stable_Fiddusion", 13);
        PyModule_AddIntConstant(module, "NoiseTypes_R2", 14);
        PyModule_AddIntConstant(module, "NoiseTypes_IGN", 15);
        PyModule_AddIntConstant(module, "NoiseTypes_Bayer", 16);
        PyModule_AddIntConstant(module, "NoiseTypes_Bayer_Plus_Half", 17);
        PyModule_AddIntConstant(module, "NoiseTypes_Round", 18);
        PyModule_AddIntConstant(module, "NoiseTypes_Floor", 19);
        PyModule_AddIntConstant(module, "NoiseTypes_White4", 20);
        PyModule_AddIntConstant(module, "NoiseTypes_White4_Plus_Half", 21);
        PyModule_AddIntConstant(module, "NoiseTypes_White8", 22);
        PyModule_AddIntConstant(module, "NoiseTypes_White8_Plus_Half", 23);
        PyModule_AddIntConstant(module, "NoiseTypes_White512", 24);
        PyModule_AddIntConstant(module, "NoiseTypes_White_Triangular", 25);
        PyModule_AddIntConstant(module, "SpatialFilters_None", 0);
        PyModule_AddIntConstant(module, "SpatialFilters_Box", 1);
        PyModule_AddIntConstant(module, "SpatialFilters_Gauss", 2);
        PyModule_AddIntConstant(module, "TemporalFilters_None", 0);
        PyModule_AddIntConstant(module, "TemporalFilters_EMA", 1);
        PyModule_AddIntConstant(module, "TemporalFilters_EMA_plus_Clamp", 2);
        PyModule_AddIntConstant(module, "TemporalFilters_Monte_Carlo", 3);
        return module;
    }
};
