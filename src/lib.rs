#![feature(core_intrinsics)]
#![feature(raw)]
#![feature(optin_builtin_traits)]
#![feature(custom_inner_attributes,custom_attribute, lang_items, panic_info_message, alloc_error_handler)]
#![feature(start)]
//#![cfg_attr(not(test), no_std)]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(warnings, unused_variables, dead_code, improper_ctypes, non_camel_case_types, non_snake_case, non_upper_case_globals)]
#![no_std]

extern crate alloc;


pub mod kernel_malloc;
#[macro_use]
pub mod macros;

pub mod raw;
pub mod lang;
pub mod io;





// Generated kernel header bindings
//pub mod kernel;
pub mod kernel_mallochelp;

pub mod allocator;

use alloc::boxed::Box;
use crate::allocator::FreebsdAllocator;

#[global_allocator]
static ALLOCATOR: FreebsdAllocator = FreebsdAllocator;


#[no_mangle]
#[start]
fn start(_argc: isize, _argv:*const *const u8) -> isize {

    //println!("Hello, start!");
    //let x = Box::new(41);
    //println!("Hello, alloc working!");
    //main();
    unsafe {
         kernel_malloc::uprintf("reallocate in place undefined!\n\0".as_ptr() as *const i8);
    }


    0
}

#[no_mangle]
pub extern "C" fn rustp() -> i32 {
    unsafe {
         kernel_malloc::uprintf("uprintf is working!\n\0".as_ptr() as *const i8);
    }
    let mut y:i32 = 0;
    unsafe {
        kernel_malloc::uprintf("printing before Box which uses malloc!\n\0".as_ptr() as *const i8);
        let x = Box::new(41);
        y = *x;       
        kernel_malloc::uprintf("printing after Box so malloc works? !\n\0".as_ptr() as *const i8);
    }


    return y as i32;

}


#[alloc_error_handler]
fn alloc_error_handler(layout: alloc::alloc::Layout) -> ! {
    panic!("allocation error: {:?}", layout)
}








// fn main() {
    
//     unsafe {
//         kernel_malloc::uprintf("reallocate in place undefined!\n\0".as_ptr() as *const i8);
//     }

//     //println!("Hello, world!");
// }
