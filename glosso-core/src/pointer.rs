//! A Little Unsafe Pointer Wrapper
//!
//! This wrapper acts just like a raw pointer with additional destructor
//! It allows to make multiple exclusive pointers but if you really dereferencing it,
//! it requires an unsafe code to do it.

use std::alloc::{alloc, dealloc, realloc, Layout};
use std::cmp::Ordering;
use std::marker::PhantomData;

/// A Little Unsafe Pointer Wrapper
#[derive(Debug)]
pub struct Pointer<T> {
    orig: Option<*mut T>,
    ptr: Option<*mut T>,
    layout: Option<Layout>,
    _marker: PhantomData<T>,
}

// Safe Functions
impl<T> Pointer<T> {
    /// Creates a new pointer that contains the given item
    ///
    /// # Example
    /// ```rust,no_run
    /// use glosso_core::pointer::Pointer;
    ///
    /// let x = Pointer::new(5);
    ///
    /// // dereferencing a Pointer is unsafe code
    /// unsafe {
    ///     println!("{}", *x.as_ptr());
    /// }
    /// ```
    pub fn new(item: T) -> Self {
        let output;

        // SAFETY: orig is first allocated from heap.
        // alloc function gurantees that the pointer which is made from it is well-aligned.
        // Now, the memory to which pointer points is initialized to the value of item.
        unsafe {
            output = Self::new_uninit();
            std::ptr::write(output.orig.unwrap(), item);
        }

        output
    }

    /// Creates a pointer that contains an array with null terminated.
    /// *len* parameter is the length of the items without the null terminated one.
    ///
    /// # Example
    ///
    /// ```rust,no_run
    /// use glosso_core::pointer::Pointer;
    ///
    /// let buffer = ["Hello", "World!"];
    /// let _ = Pointer::new_array(&buffer);
    /// ```
    pub fn new_array(items: &[T]) -> Self {
        assert!(items.len() + 1 <= usize::MAX);

        let len = items.len();
        let output;

        unsafe {
            // SAFETY: First, alocate (len + 1) * size_of::<T>() amount of memory on the heap.
            output = Self::new_uninit_array(len + 1);

            // SAFETY: Self::new_uninit_array ensures that output.orig is not null,
            // well-aligned, and has enough memory to write bytes.
            // Also we checked that items has at least len * size_of::<T>() bytes.
            std::ptr::copy_nonoverlapping(items.as_ptr(), output.orig.unwrap(), len);

            // SAFETY: Finally by adding zeros, output.orig is null-terminated.
            std::ptr::write_bytes(output.orig.unwrap().add(len), 0, 1);
        }

        output
    }

    /// Creates a null pointer.
    #[inline]
    pub fn null() -> Self {
        Self {
            orig: None,
            ptr: None,
            layout: None,
            _marker: PhantomData,
        }
    }

    pub fn reallocate(&mut self, item: T) {
        unsafe {
            self.reallocate_uninit();
            std::ptr::write(self.as_ptr(), item);
        }
    }

    /// Reallocates memory and store the buffer
    pub fn reallocate_array(&mut self, buffer: &[T]) {
        // SAFETY: the only reason that the function is unsafe is that
        // it gives an uninitalized pointer.
        // We will initialize it from the given buffer.
        // Also, we will add the null terminator.
        unsafe {
            self.reallocate_uninit_array(buffer.len() + 1);
            std::ptr::copy_nonoverlapping(buffer.as_ptr(), self.as_ptr(), buffer.len());
            std::ptr::write_bytes(self.as_ptr().add(buffer.len()), 0, 1);
        }
    }

    pub fn r#move(&mut self) -> Self {
        let layout = self.layout;
        self.layout = None;
        Self {
            orig: self.orig,
            ptr: self.ptr,
            layout,
            _marker: PhantomData,
        }
    }

    /// Check whether given pointer is null pointer.
    #[inline]
    pub fn is_null(&self) -> bool {
        self.orig == None
    }

    /// Check that the given pointer ownes an inner type.
    /// If it is true, dropping this pointer will deallocates the inner value.
    ///
    /// # Example
    ///
    /// ```rust,no_run
    /// use glosso_core::pointer::Pointer;
    ///
    /// let mut x = Pointer::new(String::from("Pointer"));
    /// {
    ///     let y = x.r#move();
    ///     assert!(y.is_owned());
    /// } // Since `y` ownes an inner value, inner string
    ///   // will be deallocated while `y` is dropped.
    ///
    /// unsafe { println!("{}", *x.as_ptr()); } // UB
    /// ```
    #[inline]
    pub fn is_owned(&self) -> bool {
        matches!(self.layout, Some(_))
    }
}

// Unsafe functions
impl<T> Pointer<T> {
    pub unsafe fn new_uninit() -> Self {
        let layout = Some(Layout::new::<T>());
        // SAFETY: Allocating a memory is actually a safe code
        let orig: *mut T = alloc(layout.unwrap()) as *mut _;

        Self {
            orig: if orig.is_null() { None } else { Some(orig) },
            ptr: if orig.is_null() { None } else { Some(orig) },
            layout,
            _marker: PhantomData,
        }
    }

    pub unsafe fn new_uninit_array(len: usize) -> Self {
        let layout = Some(Layout::array::<T>(len).unwrap());
        // SAFETY: Allocating a memory is actually a safe code
        let orig: *mut T = alloc(layout.unwrap()) as *mut _;

        Self {
            orig: if orig.is_null() { None } else { Some(orig) },
            ptr: if orig.is_null() { None } else { Some(orig) },
            layout,
            _marker: PhantomData,
        }
    }

    pub unsafe fn reallocate_uninit(&mut self) {
        match (self.orig, self.layout) {
            (Some(orig), Some(layout)) => {
                // SAFETY: Upper if clause checks that the orig is not null and the layout is
                // the proper one of the type T.
                // Frist, call the destructor (if exists) of the inner type and
                // reallocates the memory using realloc.
                // (However, there might exist some pointers that points the old orig pointer.
                // As we deallocate the old one, it now be a dangling pointer.
                // But, dereferencing a pointer is unsafe and the only way to get the inner one
                // is calling *as_ptr* method, which is also a unsafe code)
                drop(std::ptr::read_unaligned(orig));
                let new_ptr: *mut T =
                    realloc(orig as *mut _, layout, std::mem::size_of::<T>()) as *mut _;

                self.orig = if new_ptr.is_null() {
                    None
                } else {
                    Some(new_ptr)
                };
                self.ptr = self.orig;
                self.layout = Some(Layout::new::<T>());
            }
            (None, None) => {
                *self = Pointer::new_uninit();
            }
            _ => {}
        }
    }

    /// Reallocates memory by `new_size * size_of::<T>()` but uninitialized.
    ///
    /// # Safety
    /// Use `reallocate_array` instead if there is an array to store it
    pub unsafe fn reallocate_uninit_array(&mut self, new_size: usize) {
        match (self.orig, self.layout) {
            (Some(orig), Some(layout)) => {
                // SAFETY: Upper if clause checks that the orig is not null and the layout is
                // the proper one of the type T.
                // Frist, call the destructor (if exists) of the inner type and
                // reallocates the memory using realloc.
                // (However, there might exist some pointers that points the old orig pointer.
                // As we deallocate the old one, it now be a dangling pointer.
                // But, dereferencing a pointer is unsafe and the only way to get the inner one
                // is calling *as_ptr* method, which is also a unsafe code)
                drop(std::ptr::read_unaligned(orig));
                let new_ptr: *mut T =
                    realloc(orig as *mut _, layout, new_size * std::mem::size_of::<T>()) as *mut _;

                self.orig = if new_ptr.is_null() {
                    None
                } else {
                    Some(new_ptr)
                };
                self.ptr = self.orig;
                self.layout = Some(Layout::array::<T>(new_size).unwrap());
            }
            (None, None) => {
                *self = Pointer::new_uninit_array(new_size);
            }
            _ => {}
        }
    }

    /// Takes an inner raw pointer.
    ///
    /// # Panics
    /// Panics if the pointer is null
    ///
    /// # Safety
    /// Unwrapping Pointer type is unsafe because after unwrapping it,
    /// it can be copied to other codes, and if the pointee is deallocated
    /// (if the owned Pointer type is droped), every pointers will be dangle.
    ///
    /// # Example
    /// ```rust,no_run
    /// use glosso_core::pointer::Pointer;
    ///
    /// let x = Pointer::new(3);
    /// let ptr = unsafe { x.as_ptr() }; // Points 3
    /// unsafe { println!("{}", *ptr); }
    /// ```
    pub unsafe fn as_ptr(&self) -> *mut T {
        match self.ptr.as_ref() {
            Some(ptr) => *ptr,
            None => std::ptr::null_mut::<T>(),
        }
    }

    pub unsafe fn add(&self, n: usize) -> Self {
        Self {
            orig: self.orig,
            ptr: self.ptr.map(|ptr| ptr.add(n)),
            layout: None,
            _marker: PhantomData,
        }
    }

    pub unsafe fn sub(&self, n: usize) -> Self {
        Self {
            orig: self.orig,
            ptr: self.ptr.map(|ptr| ptr.sub(n)),
            layout: None,
            _marker: PhantomData,
        }
    }

    pub unsafe fn add_and_move(&mut self, n: usize) -> Self {
        let layout = self.layout;
        self.layout = None;
        Self {
            orig: self.orig,
            ptr: self.ptr.map(|ptr| ptr.add(n)),
            layout,
            _marker: PhantomData,
        }
    }

    pub unsafe fn sub_and_move(&mut self, n: usize) -> Self {
        let layout = self.layout;
        self.layout = None;
        Self {
            orig: self.orig,
            ptr: self.ptr.map(|ptr| ptr.sub(n)),
            layout,
            _marker: PhantomData,
        }
    }

    pub unsafe fn add_assign(&mut self, n: usize) {
        self.ptr = self.ptr.map(|ptr| ptr.add(n));
    }

    pub unsafe fn sub_assign(&mut self, n: usize) {
        self.ptr = self.ptr.map(|ptr| ptr.sub(n));
    }
}

impl<T> Drop for Pointer<T> {
    fn drop(&mut self) {
        if let (Some(orig), Some(layout)) = (self.orig, self.layout) {
            unsafe {
                // SAFETY: Upper if clause checks that the orig is not null and the layout is
                // the proper one of the type T.
                // Frist, call the destructor (if exists) of the inner type and
                // deallocates the memory using dealloc.
                drop(std::ptr::read_unaligned(orig));
                dealloc(orig as *mut _, layout);

                self.orig = None;
                self.ptr = None;
                self.layout = None;
            }
        }
    }
}

impl<T> Clone for Pointer<T> {
    fn clone(&self) -> Self {
        Self {
            orig: self.orig,
            ptr: self.ptr,
            layout: None,
            _marker: PhantomData,
        }
    }
}

impl<T> PartialEq for Pointer<T> {
    fn eq(&self, rhs: &Self) -> bool {
        self.orig == rhs.orig && self.ptr == rhs.ptr
    }
}

impl<T> PartialOrd for Pointer<T> {
    fn partial_cmp(&self, rhs: &Self) -> Option<Ordering> {
        if self.orig == rhs.orig {
            self.ptr.partial_cmp(&rhs.ptr)
        } else {
            None
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn create_pointer() {
        let _ = Pointer::new(5);
    } // Deallocation must work

    #[test]
    fn create_pointer_with_dyn_type() {
        let _ = Pointer::new("Hello");
    }

    #[test]
    fn copy_and_move() {
        let mut x = Pointer::new_array(&[0, 1, 2, 3]);
        let y = x.clone();
        let z = x.clone();
        {
            let w = x.r#move();
            unsafe {
                for i in 0..4 {
                    assert_eq!(*y.as_ptr().add(i), i);
                    assert_eq!(*z.as_ptr().add(i), i);
                    assert_eq!(*w.as_ptr().add(i), i);
                }
            }
        }

        assert!(!x.is_owned());
        assert!(!y.is_owned());
        assert!(!z.is_owned());
    }

    #[test]
    fn reallocate_memory_to_null() {
        let mut x = Pointer::null();
        x.reallocate(String::from("안녕"));

        assert!(!x.is_null());
        assert!(x.is_owned());
    }

    #[test]
    fn reallocate_memory() {
        let mut x = Pointer::new(String::from("안녕하세요"));
        x.reallocate(String::from("world!"));

        assert_eq!(unsafe { &*x.as_ptr() }, "world!");
    }

    #[test]
    fn reallocate_array_memory() {
        let expected = ["This", "is", "a", "box"];
        let mut x = Pointer::new_array(&["사전식", "배열"]);
        x.reallocate_array(&expected);

        assert_eq!(unsafe { (&*x.as_ptr()).len() }, 4);

        for i in 0..4 {
            assert_eq!(unsafe { *x.as_ptr().add(i) }, expected[i]);
        }
    }
}
