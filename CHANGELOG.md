# Changelog - Validador de Identidades Únicas

Todas as mudanças notáveis neste projeto serão documentadas neste arquivo.

## [2.0.0] - 2025-08-11

### 🏗️ Reestruturação Completa do Projeto
- **BREAKING**: Reorganização completa da estrutura de pastas
- Movidos arquivos fonte para `src/` com subpastas organizadas:
  - `src/server/` - Implementações dos servidores
  - `src/client/` - Cliente
  - `src/common/` - Código compartilhado
- Executáveis agora compilados para `bin/`
- Headers de protocolo movidos para `include/`

### ✨ Novas Funcionalidades
- Protocolo de comunicação em lote (`GETB <n>`)
- Suporte melhorado para múltiplos clientes simultâneos
- Documentação completa dos headers com Doxygen
- Exemplos práticos de uso em `EXAMPLES.md`

### 🚀 Melhorias de Performance
- Otimizações de compilação com LTO habilitado por padrão
- Protocolo em lote reduz overhead de rede significativamente
- Thread safety melhorada para cenários multi-cliente

### 📝 Documentação
- README.md completamente reescrito
- Documentação de API completa nos headers
- Guia de resolução de problemas
- Métricas de performance e benchmarks
- Exemplos de uso prático

### 🐛 Correções
- Corrigidos caminhos de include após reestruturação
- Makefile atualizado para nova estrutura
- Comandos de limpeza corrigidos para Windows

### 🔧 Melhorias Técnicas
- Melhor separação entre código servidor, cliente e comum
- Headers mais bem documentados com comentários Doxygen
- Protocolo de comunicação mais robusto
- Suporte melhorado para compilação cross-platform

## [1.0.0] - Data Anterior

### 🎉 Versão Inicial
- Implementação básica com três estruturas de dados:
  - Fila (Queue) - FIFO
  - Pilha (Stack) - LIFO  
  - Lista Encadeada (Linked List)
- Cliente e servidor básicos
- Protocolo de comunicação simples
- Makefile com targets debug/release
- Suporte para Windows e Linux
- Embaralhamento Fisher-Yates
- Multithreading básico

### 🏗️ Arquitetura
- Arquivos na raiz do projeto
- Headers e implementações no mesmo nível
- Protocolo de texto simples (apenas GET)
- Thread por cliente

### 📊 Performance
- Suporte para volumes de 1M a 1B IDs
- Medição de tempo de benchmark
- Testes básicos de carga

---

## Legendas
- 🎉 Nova funcionalidade
- 🏗️ Mudança de arquitetura  
- 🚀 Melhoria de performance
- 🐛 Correção de bug
- 📝 Documentação
- 🔧 Melhoria técnica
- ⚠️ Mudança que quebra compatibilidade
