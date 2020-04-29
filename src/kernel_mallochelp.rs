//#![no_std]
//#![allow(warnings, unused_variables, dead_code, improper_ctypes, non_camel_case_types, non_snake_case, non_upper_case_globals)]
// this is reference code from johalun/echo
use core::alloc::{GlobalAlloc, Layout};
use core::fmt;
use core::ptr;


#[no_mangle]
pub extern "C" fn __rust_allocate(size: usize, align: usize) -> *mut u8 {
    unsafe { imp::allocate(size, align) }
}

#[no_mangle]
pub extern "C" fn __rust_allocate_zeroed(size: usize, align: usize) -> *mut u8 {
    unsafe { imp::allocate_zeroed(size, align) }
}

#[no_mangle]
pub extern "C" fn __rust_deallocate(ptr: *mut u8, old_size: usize, align: usize) {
    unsafe { imp::deallocate(ptr, old_size, align) }
}

#[no_mangle]
pub extern "C" fn __rust_reallocate(ptr: *mut u8,
                                    old_size: usize,
                                    size: usize,
                                    align: usize)
                                    -> *mut u8 {
    unsafe { imp::reallocate(ptr, old_size, size, align) }
}

#[no_mangle]
pub extern "C" fn __rust_reallocate_inplace(ptr: *mut u8,
                                            old_size: usize,
                                            size: usize,
                                            align: usize)
                                            -> usize {
    unsafe { imp::reallocate_inplace(ptr, old_size, size, align) }
}

#[no_mangle]
pub extern "C" fn __rust_usable_size(size: usize, align: usize) -> usize {
    imp::usable_size(size, align)
}


//pub mod raw;
// for library use
//pub mod kernel_malloc;

mod imp {

    use crate::kernel_malloc as kern;
    use crate::raw;
    use core::cmp;
    use core::ptr;
    
    const MIN_ALIGN: usize = 16;  

    // pub trait LocalAlloc {
    //     unsafe fn alloc(&mut self, layout: Layout) -> *mut u8;
    //     unsafe fn dealloc(&mut self, ptr: *mut u8, layout: Layout);
    // }

    //  /// Thread-safe (locking) wrapper around a particular memory allocator.
    //  pub struct Allocator(Mutex<Option<AllocatorImpl>>);


    pub unsafe fn allocate(size: usize, align: usize) -> *mut u8 {

        // kern::uprintf("allocate: %d\n\0".as_ptr() as *const i8, size);

        if align <= MIN_ALIGN {
            kern::malloc(size as raw::c_size_t,
                         &mut kern::M_DEVBUF[0],
                         kern::M_WAITOK as i32) as *mut u8
        } else {
            aligned_malloc(size, align)
        }
    }

    pub unsafe fn allocate_zeroed(size: usize, align: usize) -> *mut u8 {

        // kern::uprintf("allocate_zeroed: %d\n\0".as_ptr() as *const i8, size);

        if align <= MIN_ALIGN {
            kern::malloc(size as raw::c_size_t,
                         &mut kern::M_DEVBUF[0],
                         kern::M_WAITOK as i32 | kern::M_ZERO as i32) as *mut u8
        } else {
            let ptr = aligned_malloc(size, align);
            if !ptr.is_null() {
                ptr::write_bytes(ptr, 0, size);
            }
            ptr
        }
    }


    unsafe fn aligned_malloc(size: usize, align: usize) -> *mut u8 {
        // TODO: Replace with proper call
        kern::uprintf("aligned_malloc: using normal malloc!\n\0".as_ptr() as *const i8);
        kern::malloc(size as raw::c_size_t,
                     &mut kern::M_DEVBUF[0],
                     kern::M_WAITOK as i32) as *mut u8
    }


    pub unsafe fn reallocate(ptr: *mut u8, old_size: usize, size: usize, align: usize) -> *mut u8 {

        // kern::uprintf("reallocate %d -> %d\n\0".as_ptr() as *const i8, old_size, size);

        if align <= MIN_ALIGN {
            kern::realloc(ptr as *mut raw::c_void,
                          size as raw::c_size_t,
                          &mut kern::M_DEVBUF[0],
                          kern::M_WAITOK as i32) as *mut u8
        } else {
            let new_ptr = allocate(size, align);
            if !new_ptr.is_null() {
                ptr::copy(ptr, new_ptr, cmp::min(size, old_size));
                deallocate(ptr, old_size, align);
            }
            new_ptr
        }
    }

    pub unsafe fn reallocate_inplace(_ptr: *mut u8,
                                     old_size: usize,
                                     _size: usize,
                                     _align: usize)
                                     -> usize {
        kern::uprintf("reallocate in place undefined!\n\0".as_ptr() as *const i8);
        old_size
    }

    pub unsafe fn deallocate(ptr: *mut u8, _old_size: usize, _align: usize) {
        // kern::uprintf("deallocate %d\n\0".as_ptr() as *const i8, _old_size);
        kern::free(ptr as *mut raw::c_void, &mut kern::M_DEVBUF[0])
    }

    pub fn usable_size(size: usize, _align: usize) -> usize {
        size
    }
}