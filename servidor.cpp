/**************************************************************
#         		Pontificia Universidad Javeriana
#     Autor: Salomon Alfredo Avila Larrotta
#     Fecha: Septiembre 2026
#     Materia: Introducción a los sistemas distribuidos
#     Tema: Ejemplo de Request/Reply en C++ con documentación
#     Fichero: Servidor ZeroMQ que implementa el reply. 
#****************************************************************/

// servidor.cpp — Servidor ZeroMQ (REP) que escucha en localhost y responde
// "pong" a cada "ping" recibido.
//
// Compilar:
//   g++ -std=c++20 servidor.cpp -lzmq -o servidor
// Ejecutar:
//   ./servidor

#include <iostream>
#include <string>

#include <zmq.hpp>

namespace ping_pong {

// Message: representa un mensaje del protocolo ping/pong.
//
// Es un TAD (tipo abstracto de datos) mínimo: solo agrupa el texto crudo
// recibido/enviado por el socket y sabe clasificarse a sí mismo como "ping"
// válido o no. Se usa `struct` (no `class`) porque es un objeto pasivo de
// datos sin invariantes entre campos.
struct Message {
  // Contenido textual del mensaje, tal como viaja por el socket.
  std::string payload;

  // Devuelve true si este mensaje es un "ping" válido.
  bool IsPing() const { return payload == "ping"; }

  // Construye el mensaje de respuesta correspondiente a este mensaje.
  // Si no es un ping reconocido, responde con un mensaje de error.
  Message Reply() const {
    return Message{IsPing() ? "pong" : "error: unknown request"};
  }
};

}  // namespace ping_pong

int main() {
  // El contexto posee el hilo de I/O interno de ZeroMQ; debe vivir mientras
  // exista el socket.
  zmq::context_t context;

  // Socket REP (reply): recibe una petición y debe responder exactamente una
  // vez antes de poder recibir la siguiente.
  zmq::socket_t socket(context, zmq::socket_type::rep);
  socket.bind("tcp://localhost:5555");

  std::cout << "[server] Escuchando en tcp://localhost:5555\n";

  while (true) {
    // Bloquea hasta que llegue una petición del cliente.
    zmq::message_t request;
    (void)socket.recv(request, zmq::recv_flags::none);

    const ping_pong::Message received{request.to_string()};
    const ping_pong::Message reply = received.Reply();

    std::cout << "[server] Recibido \"" << received.payload
              << "\", respondiendo \"" << reply.payload << "\"\n";

    socket.send(zmq::buffer(reply.payload), zmq::send_flags::none);
  }

  return 0;
}