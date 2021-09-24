use std::convert::TryInto;

pub(crate) fn read_be_i64(input: &[u8]) -> i64 {
    let (int_bytes, _) = input.split_at(std::mem::size_of::<i64>());
    i64::from_be_bytes(int_bytes.try_into().unwrap())
}

pub(crate) fn read_be_u64(input: &[u8]) -> u64 {
    let (uint_bytes, _) = input.split_at(std::mem::size_of::<u64>());
    u64::from_be_bytes(uint_bytes.try_into().unwrap())
}

pub(crate) fn read_be_f64(input: &[u8]) -> f64 {
    let (float_bytes, _) = input.split_at(std::mem::size_of::<f64>());
    f64::from_be_bytes(float_bytes.try_into().unwrap())
}

pub(crate) fn read_be_usize(input: &[u8]) -> usize {
    let (float_bytes, _) = input.split_at(std::mem::size_of::<usize>());
    usize::from_be_bytes(float_bytes.try_into().unwrap())
}
