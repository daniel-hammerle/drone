#ifndef PACKETS
#define PACKETS
struct MovePacket {
  int16_t dx;
  int16_t dy;
  int16_t dz;
  int16_t turn;
}__attribute__((packed));


union PacketUnion {
  MovePacket movePacket;
}__attribute__((packed));

struct Packet {
  PacketUnion u;
  char type;
}__attribute__((packed));

#define INS_MOVE 1
#define INS_KILL 2
#define INS_START 3

int convertEndianAndValidate(int16_t* num) {
  *num = (*num  >> 8) | (*num  << 8);
  if (*num > 10000 || *num < -10000) {
    Serial.print(*num);
    return 1;
  }
  return 0;
}

int validateMove(struct MovePacket& mp) {
  return (
    convertEndianAndValidate(&mp.dx) ||
    convertEndianAndValidate(&mp.dy) ||
    convertEndianAndValidate(&mp.dz) ||
    convertEndianAndValidate(&mp.turn)
  );
} 


int validatePacket(struct Packet& p) {
  switch (p.type) {
  case INS_MOVE:
    return validateMove(p.u.movePacket);
  case INS_START:
  case INS_KILL:
    return 0;
  default:
    Serial.println("INvalid byte");
    Serial.println(p.type);
    return 1;
  }

  //in case of success return 0
  return 0;
}
#endif
