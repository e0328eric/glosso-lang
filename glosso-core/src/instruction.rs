use num_derive::FromPrimitive;

use crate::value::Value;

macro_rules! impl_opcode {
    ($(($opcodeName: literal, $opcode: ident),)+) => {
		/// Opcodes of the glosso assembly
        #[derive(Debug, Clone, Copy, PartialEq, FromPrimitive)]
        pub enum Opcode {
            $($opcode,)+
            Illegal,
        }

        #[allow(missing_debug_implementations)]
        pub enum OperandType {
            NoOperand,
            HasOperand,
            LoopOperand,
        }

        #[derive(Debug)]
        pub struct Instruction {
            opcode: Opcode,
            operand: Value,
        }

        impl Opcode {
			/// Checkes that whether the given opcode has an operand
            #[inline]
            pub fn has_operand(&self) -> OperandType {
                match self {
                    Opcode::Push
                    | Opcode::Dup
                    | Opcode::RelativeJmp
                    | Opcode::RelativeJmpTrue
                    | Opcode::RelativeJmpFalse
                    | Opcode::RelativeJmpEq
                    | Opcode::RelativeJmpNeq
                    | Opcode::Swap
                    | Opcode::Alloc
                    | Opcode::ReAlloc => OperandType::HasOperand,
                    Opcode::Jmp
                    | Opcode::JmpTrue
                    | Opcode::JmpFalse
                    | Opcode::JmpEq
                    | Opcode::JmpNeq
                    | Opcode::Call => OperandType::LoopOperand,
                    _ => OperandType::NoOperand,
                }
            }

			/// Parse string into a corresponding opcode
			///
			/// # Example
			///
			/// ```rust,no_run
			/// use glosso_core::instruction::Opcode;
			///
			/// let opcodes_lit = "push pop alloc dup je";
			/// let expected = vec![
			///		Opcode::Push,
			///		Opcode::Pop,
			///		Opcode::Alloc,
			///		Opcode::Dup,
			///		Opcode::JmpEq,
			///	];
			///	let parsed: Vec<Opcode> = opcodes_lit
			///		.split(' ')
			///		.map(|s| Opcode::str_to_opcode(s))
			///		.collect();
			///	assert_eq!(expected, parsed);
			/// ```
            pub fn str_to_opcode(string: &str) -> Self {
                match string {
                    $($opcodeName => Self::$opcode,)+
                    _ => Self::Illegal,
                }
            }
        }
    };
}

impl_opcode![
    ("nop", Nop),
    ("push", Push),
    ("pushf", PushFalse),
    ("pusht", PushTrue),
    ("push0", PushZero),
    ("push1", PushOne),
    ("pushn1", PushNegOne),
    ("pushu0", PushUZero),
    ("pushu1", PushUOne),
    ("pushf0", PushFZero),
    ("pushf1", PushFOne),
    ("pushfn1", PushFNegOne),
    ("pop", Pop),
    ("dup", Dup),
    ("jmp", Jmp),
    ("jt", JmpTrue),
    ("jf", JmpFalse),
    ("je", JmpEq),
    ("jne", JmpNeq),
    ("rjmp", RelativeJmp),
    ("rjt", RelativeJmpTrue),
    ("rjf", RelativeJmpFalse),
    ("rje", RelativeJmpEq),
    ("rjne", RelativeJmpNeq),
    ("call", Call),
    ("ret", Return),
    ("swap", Swap),
    ("not", Not),
    ("add", Add),
    ("sub", Sub),
    ("mul", Mul),
    ("div", Div),
    ("neg", Negate),
    ("eq", Equal),
    ("neq", Neq),
    ("lt", Lt),
    ("lte", LtEq),
    ("gt", Gt),
    ("gte", GtEq),
    ("inc", Inc),
    ("dec", Dec),
    ("scani", ScanI),
    ("scanu", ScanU),
    ("scanf", ScanF),
    ("scanc", ScanC),
    ("scanb", ScanB),
    ("scans", ScanS),
    ("print", Print),
    ("println", PrintLn),
    ("prints", PrintS),
    ("printsln", PrintSLn),
    ("alloc", Alloc),
    ("realloc", ReAlloc),
    ("free", Free),
    ("readi", ReadI),
    ("readu", ReadU),
    ("readf", ReadF),
    ("readc", ReadC),
    ("readb", ReadB),
    ("writei", WriteI),
    ("writeu", WriteU),
    ("writef", WriteF),
    ("writec", WriteC),
    ("writeb", WriteB),
    ("i2u", I2U),
    ("i2f", I2F),
    ("i2c", I2C),
    ("u2i", U2I),
    ("u2f", U2F),
    ("u2c", U2C),
    ("u2b", U2B),
    ("f2i", F2I),
    ("f2u", F2U),
    ("f2c", F2C),
    ("c2i", C2I),
    ("c2u", C2U),
    ("c2f", C2F),
    ("b2u", B2U),
    ("n2b", N2B),
    ("halt", Halt),
];

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parse_opcode() {
        let opcodes_lit = "push pop alloc dup je";
        let expected = vec![
            Opcode::Push,
            Opcode::Pop,
            Opcode::Alloc,
            Opcode::Dup,
            Opcode::JmpEq,
        ];
        let parsed: Vec<Opcode> = opcodes_lit
            .split(' ')
            .map(|s| Opcode::str_to_opcode(s))
            .collect();
        assert_eq!(expected, parsed);
    }
}
