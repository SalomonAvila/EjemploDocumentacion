/**************************************************************
#         		Pontificia Universidad Javeriana
#     Autor: Salomon Alfredo Avila Larrotta
#     Fecha: Septiembre 2026
#     Materia: Introducción a los sistemas distribuidos
#     Tema: Ejemplo de Request/Reply en C++ con documentación
#     Fichero: Cliente ZeroMQ que implementa el request. 
#****************************************************************/


// cliente.cpp — Cliente ZeroMQ (REQ) que se conecta a localhost y envía
// "ping", esperando recibir "pong".
//
// Compilar:
//   g++ -std=c++20 cliente.cpp -lzmq -o cliente
// Ejecutar (con el servidor ya corriendo):
//   ./client

#include <iostream>
#include <string>

#include <zmq.hpp>

namespace ping_pong {

// Message: representa un mensaje del protocolo ping/pong.
//
// Mismo TAD que usa servidor.cpp: agrupa el texto del mensaje y sabe construir
// la petición de ping que el cliente debe enviar.
struct Message {
  // Contenido textual del mensaje, tal como viaja por el socket.
  std::string payload;

  // Fábrica: construye el mensaje "ping" que el cliente envía al servidor.
  static Message Ping() { return Message{"ping"}; }
};

}  // namespace ping_pong

int main() {
  // El contexto posee el hilo de I/O interno de ZeroMQ; debe vivir mientras
  // exista el socket.
  zmq::context_t context;

  // Socket REQ (request): envía una petición y debe esperar la respuesta
  // antes de poder enviar la siguiente.
  zmq::socket_t socket(context, zmq::socket_type::req);
  socket.connect("tcp://localhost:5555");

  const ping_pong::Message ping = ping_pong::Message::Ping();

  std::cout << "Cliente Enviando \"" << ping.payload << "\"\n";
  socket.send(zmq::buffer(ping.payload), zmq::send_flags::none);

  // Bloquea hasta que llegue la respuesta del servidor.
  zmq::message_t reply;
  (void)socket.recv(reply, zmq::recv_flags::none);

  std::cout << "Cliente recibido \"" << reply.to_string() << "\"\n";

  return 0;
}