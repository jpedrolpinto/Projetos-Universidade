import jsonServer from 'json-server';
const server = jsonServer.create();
const router = jsonServer.router('trabalhodb.json');
const middlewares = jsonServer.defaults();

// Definir a porta
const port = 3000;

// Usar os middlewares padrão (logger, static, cors and no-cache)
server.use(middlewares);

// Usar o router do json-server
server.use(router);

// Iniciar o servidor
server.listen(port, () => {
  console.log(`JSON Server está rodando na porta ${port}`);
  console.log(`http://localhost:${port}`);
});