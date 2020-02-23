//
//  data_instrs.h
//  Katara
//
//  Created by Arne Philipeit on 12/17/19.
//  Copyright © 2019 Arne Philipeit. All rights reserved.
//

#ifndef x86_64_data_instrs_h
#define x86_64_data_instrs_h

#include <memory>
#include <string>

#include "common/data.h"
#include "x86_64/mc/linker.h"
#include "x86_64/mc/unlinker.h"
#include "x86_64/instr.h"
#include "x86_64/ops.h"
#include "x86_64/instrs/cf_instrs.h"

namespace x86_64 {

class Mov final : public Instr {
public:
    Mov(RM dst, Operand src);
    ~Mov() override;
    
    RM dst() const;
    Operand src() const;
    
    int8_t Encode(Linker *linker,
                  common::data code) const override;
    std::string ToString() const override;

private:
    typedef enum : uint8_t {
        kRM_REG,
        kREG_RM,
        kREG_IMM,
        kRM_IMM
    } MovType;
    
    MovType mov_type_;
    RM dst_;
    Operand src_;
};

class Xchg final : public Instr {
    Xchg(RM rm, Reg reg);
    ~Xchg() override;
    
    RM op_a() const;
    Reg op_b() const;
    
    int8_t Encode(Linker *linker,
                  common::data code) const override;
    std::string ToString() const override;
    
private:
    bool CanUseRegAShortcut() const;
    
    RM op_a_;
    Reg op_b_;
};

class Push final : public Instr {
public:
    Push(RM rm);
    Push(Imm imm);
    ~Push() override;
    
    Operand op() const;
    
    int8_t Encode(Linker *linker,
                  common::data code) const override;
    std::string ToString() const override;

private:
    Operand op_;
};

class Pop final : public Instr {
public:
    Pop(RM rm);
    ~Pop() override;
    
    RM op() const;
    
    int8_t Encode(Linker *linker,
                  common::data code) const override;
    std::string ToString() const override;
    
private:
    RM op_;
};

class Setcc final : public Instr {
public:
    Setcc(InstrCond cond, RM op);
    ~Setcc() override;
    
    InstrCond cond() const;
    RM op() const;
    
    int8_t Encode(Linker *linker,
                  common::data code) const override;
    std::string ToString() const override;
    
private:
    InstrCond cond_;
    RM op_;
};

}

#endif /* x86_64_data_instrs_h */
