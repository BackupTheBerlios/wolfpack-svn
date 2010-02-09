
TEMPLATE = lib
TARGET = boost_python
DESTDIR = lib

# Include configure settings
!include(../build/config.pri) {
	message("HINT: use ./configure.py script!")
}

CONFIG -= qt
CONFIG += staticlib warn_off release exceptions rtti

DEFINES += BOOST_PYTHON_STATIC_LIB BOOST_PYTHON_SOURCE BOOST_ALL_NO_LIB

INCLUDEPATH += include $$PY_INCDIR

DEPENDPATH += . \
              python \
              python\converter \
              python\object \
              include\boost\python \
              include\boost\python\converter \
              include\boost\python\detail \
              include\boost\python\object \
              include\boost\python\suite\indexing \
              include\boost\python\suite\indexing\detail
INCLUDEPATH += .

# Input
HEADERS += include/boost/python/arg_from_python.hpp \
           include/boost/python/args.hpp \
           include/boost/python/args_fwd.hpp \
           include/boost/python/back_reference.hpp \
           include/boost/python/base_type_traits.hpp \
           include/boost/python/bases.hpp \
           include/boost/python/borrowed.hpp \
           include/boost/python/call.hpp \
           include/boost/python/call_method.hpp \
           include/boost/python/cast.hpp \
           include/boost/python/class.hpp \
           include/boost/python/class_fwd.hpp \
           include/boost/python/copy_const_reference.hpp \
           include/boost/python/copy_non_const_reference.hpp \
           include/boost/python/data_members.hpp \
           include/boost/python/def.hpp \
           include/boost/python/def_visitor.hpp \
           include/boost/python/default_call_policies.hpp \
           include/boost/python/dict.hpp \
           include/boost/python/enum.hpp \
           include/boost/python/errors.hpp \
           include/boost/python/exception_translator.hpp \
           include/boost/python/extract.hpp \
           include/boost/python/handle.hpp \
           include/boost/python/handle_fwd.hpp \
           include/boost/python/has_back_reference.hpp \
           include/boost/python/implicit.hpp \
           include/boost/python/init.hpp \
           include/boost/python/instance_holder.hpp \
           include/boost/python/iterator.hpp \
           include/boost/python/list.hpp \
           include/boost/python/long.hpp \
           include/boost/python/lvalue_from_pytype.hpp \
           include/boost/python/make_constructor.hpp \
           include/boost/python/make_function.hpp \
           include/boost/python/manage_new_object.hpp \
           include/boost/python/module.hpp \
           include/boost/python/module_init.hpp \
           include/boost/python/numeric.hpp \
           include/boost/python/object.hpp \
           include/boost/python/object_attributes.hpp \
           include/boost/python/object_call.hpp \
           include/boost/python/object_core.hpp \
           include/boost/python/object_fwd.hpp \
           include/boost/python/object_items.hpp \
           include/boost/python/object_operators.hpp \
           include/boost/python/object_protocol.hpp \
           include/boost/python/object_protocol_core.hpp \
           include/boost/python/object_slices.hpp \
           include/boost/python/opaque_pointer_converter.hpp \
           include/boost/python/operators.hpp \
           include/boost/python/other.hpp \
           include/boost/python/overloads.hpp \
           include/boost/python/override.hpp \
           include/boost/python/pointee.hpp \
           include/boost/python/proxy.hpp \
           include/boost/python/ptr.hpp \
           include/boost/python/pure_virtual.hpp \
           include/boost/python/raw_function.hpp \
           include/boost/python/refcount.hpp \
           include/boost/python/reference_existing_object.hpp \
           include/boost/python/register_ptr_to_python.hpp \
           include/boost/python/return_arg.hpp \
           include/boost/python/return_by_value.hpp \
           include/boost/python/return_internal_reference.hpp \
           include/boost/python/return_opaque_pointer.hpp \
           include/boost/python/return_value_policy.hpp \
           include/boost/python/scope.hpp \
           include/boost/python/self.hpp \
           include/boost/python/signature.hpp \
           include/boost/python/slice.hpp \
           include/boost/python/slice_nil.hpp \
           include/boost/python/str.hpp \
           include/boost/python/tag.hpp \
           include/boost/python/to_python_converter.hpp \
           include/boost/python/to_python_indirect.hpp \
           include/boost/python/to_python_value.hpp \
           include/boost/python/tuple.hpp \
           include/boost/python/type_id.hpp \
           include/boost/python/with_custodian_and_ward.hpp \
           include/boost/python/wrapper.hpp \
           include/boost/python/converter/arg_from_python.hpp \
           include/boost/python/converter/arg_to_python.hpp \
           include/boost/python/converter/arg_to_python_base.hpp \
           include/boost/python/converter/as_to_python_function.hpp \
           include/boost/python/converter/builtin_converters.hpp \
           include/boost/python/converter/constructor_function.hpp \
           include/boost/python/converter/context_result_converter.hpp \
           include/boost/python/converter/convertible_function.hpp \
           include/boost/python/converter/from_python.hpp \
           include/boost/python/converter/implicit.hpp \
           include/boost/python/converter/obj_mgr_arg_from_python.hpp \
           include/boost/python/converter/object_manager.hpp \
           include/boost/python/converter/pointer_type_id.hpp \
           include/boost/python/converter/pyobject_traits.hpp \
           include/boost/python/converter/pyobject_type.hpp \
           include/boost/python/converter/pytype_object_mgr_traits.hpp \
           include/boost/python/converter/registered.hpp \
           include/boost/python/converter/registered_pointee.hpp \
           include/boost/python/converter/registrations.hpp \
           include/boost/python/converter/registry.hpp \
           include/boost/python/converter/return_from_python.hpp \
           include/boost/python/converter/rvalue_from_python_data.hpp \
           include/boost/python/converter/shared_ptr_deleter.hpp \
           include/boost/python/converter/shared_ptr_from_python.hpp \
           include/boost/python/converter/shared_ptr_to_python.hpp \
           include/boost/python/converter/to_python_function_type.hpp \
           include/boost/python/detail/aix_init_module.hpp \
           include/boost/python/detail/api_placeholder.hpp \
           include/boost/python/detail/borrowed_ptr.hpp \
           include/boost/python/detail/caller.hpp \
           include/boost/python/detail/config.hpp \
           include/boost/python/detail/construct.hpp \
           include/boost/python/detail/convertible.hpp \
           include/boost/python/detail/copy_ctor_mutates_rhs.hpp \
           include/boost/python/detail/cv_category.hpp \
           include/boost/python/detail/dealloc.hpp \
           include/boost/python/detail/decorated_type_id.hpp \
           include/boost/python/detail/decref_guard.hpp \
           include/boost/python/detail/def_helper.hpp \
           include/boost/python/detail/def_helper_fwd.hpp \
           include/boost/python/detail/defaults_def.hpp \
           include/boost/python/detail/defaults_gen.hpp \
           include/boost/python/detail/dependent.hpp \
           include/boost/python/detail/destroy.hpp \
           include/boost/python/detail/enable_if.hpp \
           include/boost/python/detail/exception_handler.hpp \
           include/boost/python/detail/force_instantiate.hpp \
           include/boost/python/detail/if_else.hpp \
           include/boost/python/detail/indirect_traits.hpp \
           include/boost/python/detail/invoke.hpp \
           include/boost/python/detail/is_auto_ptr.hpp \
           include/boost/python/detail/is_shared_ptr.hpp \
           include/boost/python/detail/is_wrapper.hpp \
           include/boost/python/detail/is_xxx.hpp \
           include/boost/python/detail/make_keyword_range_fn.hpp \
           include/boost/python/detail/make_tuple.hpp \
           include/boost/python/detail/map_entry.hpp \
           include/boost/python/detail/mpl_lambda.hpp \
           include/boost/python/detail/msvc_typeinfo.hpp \
           include/boost/python/detail/none.hpp \
           include/boost/python/detail/not_specified.hpp \
           include/boost/python/detail/nullary_function_adaptor.hpp \
           include/boost/python/detail/operator_id.hpp \
           include/boost/python/detail/overloads_fwd.hpp \
           include/boost/python/detail/pointee.hpp \
           include/boost/python/detail/prefix.hpp \
           include/boost/python/detail/preprocessor.hpp \
           include/boost/python/detail/python22_fixed.h \
           include/boost/python/detail/raw_pyobject.hpp \
           include/boost/python/detail/referent_storage.hpp \
           include/boost/python/detail/result.hpp \
           include/boost/python/detail/scope.hpp \
           include/boost/python/detail/sfinae.hpp \
           include/boost/python/detail/signature.hpp \
           include/boost/python/detail/string_literal.hpp \
           include/boost/python/detail/target.hpp \
           include/boost/python/detail/translate_exception.hpp \
           include/boost/python/detail/type_list.hpp \
           include/boost/python/detail/type_list_impl.hpp \
           include/boost/python/detail/type_list_impl_no_pts.hpp \
           include/boost/python/detail/unwrap_type_id.hpp \
           include/boost/python/detail/unwrap_wrapper.hpp \
           include/boost/python/detail/value_arg.hpp \
           include/boost/python/detail/value_is_shared_ptr.hpp \
           include/boost/python/detail/value_is_xxx.hpp \
           include/boost/python/detail/void_ptr.hpp \
           include/boost/python/detail/void_return.hpp \
           include/boost/python/detail/wrap_python.hpp \
           include/boost/python/detail/wrapper_base.hpp \
           include/boost/python/object/add_to_namespace.hpp \
           include/boost/python/object/class.hpp \
           include/boost/python/object/class_detail.hpp \
           include/boost/python/object/class_metadata.hpp \
           include/boost/python/object/class_wrapper.hpp \
           include/boost/python/object/enum_base.hpp \
           include/boost/python/object/find_instance.hpp \
           include/boost/python/object/forward.hpp \
           include/boost/python/object/function.hpp \
           include/boost/python/object/function_handle.hpp \
           include/boost/python/object/function_object.hpp \
           include/boost/python/object/inheritance.hpp \
           include/boost/python/object/inheritance_query.hpp \
           include/boost/python/object/instance.hpp \
           include/boost/python/object/iterator.hpp \
           include/boost/python/object/iterator_core.hpp \
           include/boost/python/object/life_support.hpp \
           include/boost/python/object/make_holder.hpp \
           include/boost/python/object/make_instance.hpp \
           include/boost/python/object/make_ptr_instance.hpp \
           include/boost/python/object/pickle_support.hpp \
           include/boost/python/object/pointer_holder.hpp \
           include/boost/python/object/py_function.hpp \
           include/boost/python/object/value_holder.hpp \
           include/boost/python/object/value_holder_fwd.hpp \
           include/boost/python/suite/indexing/container_utils.hpp \
           include/boost/python/suite/indexing/indexing_suite.hpp \
           include/boost/python/suite/indexing/map_indexing_suite.hpp \
           include/boost/python/suite/indexing/vector_indexing_suite.hpp \
           include/boost/python/suite/indexing/detail/indexing_suite_detail.hpp

include( python/python.pri )