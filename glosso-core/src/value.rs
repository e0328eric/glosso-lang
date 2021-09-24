use std::cmp::Ordering;
use std::fmt::{self, Debug};
use std::io::Write;
use std::ops::{Add, AddAssign, BitAnd, BitOr, Div, Mul, Neg, Not, Sub, SubAssign};

use crate::error::{self, GlossoCoreErr};
use crate::pointer::Pointer;
use crate::utility;

#[derive(Clone)]
pub enum Value {
    Null,
    Integer(i64),
    UInteger(u64),
    Float(f64),
    // TODO: change char into the Rust's char, not the C's one
    // Since this code is rewritten from C++, to write easily, it is implemented
    // with the C++'s manner.
    Char(u8),
    Boolean(bool),
    GlobalPtr(usize),
    HeapPtr(Pointer<u8>),
}

impl Value {
    #[inline]
    pub fn is_falsity(&self) -> bool {
        matches!(self, Self::Null | Self::Boolean(false))
    }

    pub fn read_value(bytes: &[u8]) -> error::Result<(Self, usize)> {
        use std::mem::size_of;

        match bytes {
            &[0, ..] => Ok((Value::Null, 1)),
            &[1, ref rest @ ..] => {
                let value = utility::read_be_i64(rest);
                Ok((Value::Integer(value), size_of::<i64>() + 1))
            }
            &[2, ref rest @ ..] => {
                let value = utility::read_be_u64(rest);
                Ok((Value::UInteger(value), size_of::<i64>() + 1))
            }
            &[3, ref rest @ ..] => {
                let value = utility::read_be_f64(rest);
                Ok((Value::Float(value), size_of::<i64>() + 1))
            }
            &[4, value, ..] => Ok((Value::Char(value), 2)),
            &[5, value, ..] => Ok((Value::Boolean(if value == 0 { false } else { true }), 2)),
            &[6, ref rest @ ..] => {
                let value = utility::read_be_usize(rest);
                Ok((Value::GlobalPtr(value), size_of::<usize>() + 1))
            }
            _ => Err(GlossoCoreErr::ReadValueFailedErr),
        }
    }

    pub fn read_values(bytes: &[u8]) -> error::Result<Vec<Self>> {
        let mut output: Vec<Value> = Vec::with_capacity(bytes.len());
        let mut tmp = bytes;
        while !tmp.is_empty() {
            let (value, next) = Value::read_value(tmp)?;
            output.push(value);
            tmp = &tmp[next..];
        }
        output.shrink_to_fit();

        Ok(output)
    }

    pub fn write_value(&self, fs: &mut impl Write) -> error::Result<()> {
        match self {
            Self::Null => {
                fs.write(&[0])?;
            }
            Self::Integer(n) => {
                fs.write(&[1])?;
                fs.write(&n.to_be_bytes())?;
            }
            Self::UInteger(n) => {
                fs.write(&[2])?;
                fs.write(&n.to_be_bytes())?;
            }
            Self::Float(n) => {
                fs.write(&[3])?;
                fs.write(&n.to_be_bytes())?;
            }
            Self::Char(n) => {
                fs.write(&[4, *n])?;
            }
            Self::Boolean(n) => {
                fs.write(&[5, *n as u8])?;
            }
            Self::GlobalPtr(n) => {
                fs.write(&[6])?;
                fs.write(&n.to_be_bytes())?;
            }
            _ => return Err(GlossoCoreErr::TryWriteIllegalValueErr),
        }

        Ok(())
    }
}

impl Debug for Value {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            Self::Null => write!(f, "null"),
            Self::Integer(n) => write!(f, "Integer({})", n),
            Self::UInteger(n) => write!(f, "UInteger({})", n),
            Self::Float(n) => write!(f, "Float({})", n),
            Self::Char(n) => write!(f, "Char({})", n),
            Self::Boolean(n) => write!(f, "Boolean({})", n),
            Self::GlobalPtr(n) => write!(f, "GlobalPtr({})", n),
            Self::HeapPtr(ptr) => write!(f, "HeapPtr({:?})", ptr),
        }
    }
}

// Operator Overloadings
impl PartialEq for Value {
    fn eq(&self, rhs: &Self) -> bool {
        self.partial_cmp(rhs) == Some(Ordering::Equal)
    }
}

impl PartialOrd for Value {
    fn partial_cmp(&self, rhs: &Self) -> Option<Ordering> {
        match (self, rhs) {
            (Self::Null, Self::Null) => Some(Ordering::Equal),
            (Self::Null, Self::Boolean(b2)) => false.partial_cmp(b2),

            (Self::Integer(n1), Self::Integer(n2)) => n1.partial_cmp(n2),
            (Self::Integer(n1), Self::UInteger(n2)) => n1.partial_cmp(&(*n2 as i64)),
            (Self::Integer(n1), Self::Float(n2)) => (*n1 as f64).partial_cmp(n2),
            (Self::Integer(n1), Self::Char(n2)) => n1.partial_cmp(&(*n2 as i64)),

            (Self::UInteger(n1), Self::Integer(n2)) => (*n1 as i64).partial_cmp(n2),
            (Self::UInteger(n1), Self::UInteger(n2)) => n1.partial_cmp(n2),
            (Self::UInteger(n1), Self::Float(n2)) => (*n1 as f64).partial_cmp(n2),
            (Self::UInteger(n1), Self::Char(n2)) => n1.partial_cmp(&(*n2 as u64)),

            (Self::Float(n1), Self::Integer(n2)) => n1.partial_cmp(&(*n2 as f64)),
            (Self::Float(n1), Self::UInteger(n2)) => n1.partial_cmp(&(*n2 as f64)),
            (Self::Float(n1), Self::Float(n2)) => n1.partial_cmp(n2),
            (Self::Float(n1), Self::Char(n2)) => n1.partial_cmp(&(*n2 as f64)),

            (Self::Char(n1), Self::Integer(n2)) => (*n1 as i64).partial_cmp(n2),
            (Self::Char(n1), Self::UInteger(n2)) => (*n1 as u64).partial_cmp(n2),
            (Self::Char(n1), Self::Float(n2)) => (*n1 as f64).partial_cmp(n2),
            (Self::Char(n1), Self::Char(n2)) => n1.partial_cmp(n2),

            (Self::Boolean(b1), Self::Null) => false.partial_cmp(b1),
            (Self::Boolean(b1), Self::Boolean(b2)) => b1.partial_cmp(b2),

            (Self::GlobalPtr(n1), Self::GlobalPtr(n2)) => n1.partial_cmp(n2),

            (Self::HeapPtr(p1), Self::HeapPtr(p2)) => p1.partial_cmp(p2),

            _ => None,
        }
    }
}

impl Add for Value {
    type Output = Self;
    fn add(self, rhs: Self) -> Self::Output {
        match (self, rhs) {
            (Self::Integer(n1), Self::Integer(n2)) => Self::Integer(n1 + n2),
            (Self::Integer(n1), Self::UInteger(n2)) => Self::Integer(n1 + n2 as i64),
            (Self::Integer(n1), Self::Float(n2)) => Self::Float(n1 as f64 + n2),

            (Self::UInteger(n1), Self::Integer(n2)) => Self::Integer(n1 as i64 + n2),
            (Self::UInteger(n1), Self::UInteger(n2)) => Self::UInteger(n1 + n2),
            (Self::UInteger(n1), Self::Float(n2)) => Self::Float(n1 as f64 + n2),

            (Self::Float(n1), Self::Integer(n2)) => Self::Float(n1 + n2 as f64),
            (Self::Float(n1), Self::UInteger(n2)) => Self::Float(n1 + n2 as f64),
            (Self::Float(n1), Self::Float(n2)) => Self::Float(n1 + n2),

            // SAFETY: by moving an ownership of the privious pointer, result pointer
            // is not dangling.
            // Also, these arithmetic will executed on the glosso language runtime,
            // not the glossovm itself.
            (Self::HeapPtr(mut ptr), Self::Integer(n)) => unsafe {
                Self::HeapPtr(ptr.add_and_move(n as usize))
            },
            (Self::HeapPtr(mut ptr), Self::UInteger(n)) => unsafe {
                Self::HeapPtr(ptr.add_and_move(n as usize))
            },

            _ => Self::Null,
        }
    }
}

impl Sub for Value {
    type Output = Self;
    fn sub(self, rhs: Self) -> Self::Output {
        match (self, rhs) {
            (Self::Integer(n1), Self::Integer(n2)) => Self::Integer(n1 - n2),
            (Self::Integer(n1), Self::UInteger(n2)) => Self::Integer(n1 - n2 as i64),
            (Self::Integer(n1), Self::Float(n2)) => Self::Float(n1 as f64 - n2),

            (Self::UInteger(n1), Self::Integer(n2)) => Self::Integer(n1 as i64 - n2),
            (Self::UInteger(n1), Self::UInteger(n2)) => Self::UInteger(n1 - n2),
            (Self::UInteger(n1), Self::Float(n2)) => Self::Float(n1 as f64 - n2),

            (Self::Float(n1), Self::Integer(n2)) => Self::Float(n1 - n2 as f64),
            (Self::Float(n1), Self::UInteger(n2)) => Self::Float(n1 - n2 as f64),
            (Self::Float(n1), Self::Float(n2)) => Self::Float(n1 - n2),

            // SAFETY: by moving an ownership of the privious pointer, result pointer
            // is not dangling.
            // Also, these arithmetic will executed on the glosso language runtime,
            // not the glossovm itself.
            (Self::HeapPtr(mut ptr), Self::Integer(n)) => unsafe {
                Self::HeapPtr(ptr.sub_and_move(n as usize))
            },
            (Self::HeapPtr(mut ptr), Self::UInteger(n)) => unsafe {
                Self::HeapPtr(ptr.sub_and_move(n as usize))
            },

            _ => Self::Null,
        }
    }
}

impl Mul for Value {
    type Output = Self;
    fn mul(self, rhs: Self) -> Self::Output {
        match (self, rhs) {
            (Self::Integer(n1), Self::Integer(n2)) => Self::Integer(n1 * n2),
            (Self::Integer(n1), Self::UInteger(n2)) => Self::Integer(n1 * n2 as i64),
            (Self::Integer(n1), Self::Float(n2)) => Self::Float(n1 as f64 * n2),

            (Self::UInteger(n1), Self::Integer(n2)) => Self::Integer(n1 as i64 * n2),
            (Self::UInteger(n1), Self::UInteger(n2)) => Self::UInteger(n1 * n2),
            (Self::UInteger(n1), Self::Float(n2)) => Self::Float(n1 as f64 * n2),

            (Self::Float(n1), Self::Integer(n2)) => Self::Float(n1 * n2 as f64),
            (Self::Float(n1), Self::UInteger(n2)) => Self::Float(n1 * n2 as f64),
            (Self::Float(n1), Self::Float(n2)) => Self::Float(n1 * n2),

            _ => Self::Null,
        }
    }
}

impl Div for Value {
    type Output = Self;
    fn div(self, rhs: Self) -> Self::Output {
        match (self, rhs) {
            (Self::Integer(n1), Self::Integer(n2)) => Self::Integer(n1 / n2),
            (Self::Integer(n1), Self::UInteger(n2)) => Self::Integer(n1 / n2 as i64),
            (Self::Integer(n1), Self::Float(n2)) => Self::Float(n1 as f64 / n2),

            (Self::UInteger(n1), Self::Integer(n2)) => Self::Integer(n1 as i64 / n2),
            (Self::UInteger(n1), Self::UInteger(n2)) => Self::UInteger(n1 / n2),
            (Self::UInteger(n1), Self::Float(n2)) => Self::Float(n1 as f64 / n2),

            (Self::Float(n1), Self::Integer(n2)) => Self::Float(n1 / n2 as f64),
            (Self::Float(n1), Self::UInteger(n2)) => Self::Float(n1 / n2 as f64),
            (Self::Float(n1), Self::Float(n2)) => Self::Float(n1 / n2),

            _ => Self::Null,
        }
    }
}

impl Neg for Value {
    type Output = Value;
    fn neg(self) -> Self::Output {
        match self {
            Self::Integer(n) => Self::Integer(-n),
            Self::Float(n) => Self::Float(-n),
            _ => Self::Null,
        }
    }
}

impl Not for Value {
    type Output = Value;
    fn not(self) -> Self::Output {
        if let Value::Boolean(n) = self {
            Self::Boolean(!n)
        } else {
            Self::Null
        }
    }
}

impl BitAnd for Value {
    type Output = Value;
    fn bitand(self, rhs: Self) -> Self::Output {
        match (self, rhs) {
            (Value::Boolean(n1), Value::Boolean(n2)) => Value::Boolean(n1 && n2),
            _ => Value::Null,
        }
    }
}

impl BitOr for Value {
    type Output = Value;
    fn bitor(self, rhs: Self) -> Self::Output {
        match (self, rhs) {
            (Value::Boolean(n1), Value::Boolean(n2)) => Value::Boolean(n1 || n2),
            _ => Value::Null,
        }
    }
}

impl AddAssign<usize> for Value {
    fn add_assign(&mut self, rhs: usize) {
        match self {
            Self::Integer(ref mut n) => *n += rhs as i64,
            Self::UInteger(ref mut n) => *n += rhs as u64,
            Self::Float(ref mut n) => *n += rhs as f64,
            Self::Char(ref mut n) => *n += rhs as u8,
            // SAFETY: arithmetic will executed on the glosso language runtime,
            // not the glossovm itself.
            Self::HeapPtr(ref mut ptr) => unsafe { ptr.add_assign(rhs) },
            _ => {}
        }
    }
}

impl SubAssign<usize> for Value {
    fn sub_assign(&mut self, rhs: usize) {
        match self {
            Self::Integer(ref mut n) => *n -= rhs as i64,
            Self::UInteger(ref mut n) => *n -= rhs as u64,
            Self::Float(ref mut n) => *n -= rhs as f64,
            Self::Char(ref mut n) => *n -= rhs as u8,
            // SAFETY: arithmetic will executed on the glosso language runtime,
            // not the glossovm itself.
            Self::HeapPtr(ref mut ptr) => unsafe { ptr.sub_assign(rhs) },
            _ => {}
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn value_arithmetic1() {
        assert_eq!(
            -Value::Integer(3) + Value::UInteger(5) * Value::Float(4.5),
            Value::Float(19.5)
        );
    }

    #[test]
    fn value_arithmetic2() {
        assert_eq!(
            !Value::Boolean(false) & Value::Boolean(true),
            Value::Boolean(true)
        );
    }

    #[test]
    fn read_and_write_value() -> error::Result<()> {
        let values = [
            Value::UInteger(3),
            Value::Float(3.14),
            Value::Integer(-5),
            Value::Null,
            Value::GlobalPtr(32),
            Value::Boolean(true),
            Value::UInteger(u64::MAX),
            Value::Integer(312312451),
        ];

        let mut buffer: Vec<u8> = Vec::with_capacity(values.len());
        for val in &values {
            val.write_value(&mut buffer)?;
        }

        println!("{:?}", buffer);

        let got = Value::read_values(&buffer)?;

        assert_eq!(values.len(), got.len());
        for i in 0..got.len() {
            assert_eq!(values[i], got[i]);
        }

        Ok(())
    }
}
