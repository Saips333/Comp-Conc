/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 11 */
/* Codigo: Exemplo de uso de futures */
/* -------------------------------------------------------------------*/

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import java.util.ArrayList;
import java.util.List;


//classe callable para verificar se um número é primo
class PrimoCallable implements Callable<Integer> {
  private long numero;

  //construtor
  PrimoCallable(long n) {
    this.numero = n;
  }

  //método para verificar se é primo
  private boolean ehPrimo(long n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (long i = 3; i <= Math.sqrt(n) + 1; i += 2) {
      if (n % i == 0) return false;
    }
    return true;
  }

  //método para execução - retorna 1 se primo, 0 se não
  public Integer call() throws Exception {
    return ehPrimo(numero) ? 1 : 0;
  }
}

//classe do método main
public class FutureHello  {
  private static final int N = 10000; //valor padrão para contar primos de 1 a N
  private static final int NTHREADS = 10;

  public static void main(String[] args) {
    //permite passar N como argumento
    int limite = N;
    if (args.length > 0) {
      limite = Integer.parseInt(args[0]);
    }

    //cria um pool de threads (NTHREADS)
    ExecutorService executor = Executors.newFixedThreadPool(NTHREADS);
    //cria uma lista para armazenar referencias de chamadas assincronas
    List<Future<Integer>> list = new ArrayList<Future<Integer>>();

    //submete tarefas para verificar cada número de 1 a N
    for (int i = 1; i <= limite; i++) {
      Callable<Integer> worker = new PrimoCallable(i);
      Future<Integer> submit = executor.submit(worker);
      list.add(submit);
    }

    System.out.println("Total de tarefas: " + list.size());

    //recupera os resultados e conta quantos são primos
    int totalPrimos = 0;
    for (Future<Integer> future : list) {
      try {
        totalPrimos += future.get(); //bloqueia se a computação nao tiver terminado
      } catch (InterruptedException e) {
        e.printStackTrace();
      } catch (ExecutionException e) {
        e.printStackTrace();
      }
    }
    System.out.println("Quantidade de primos de 1 a " + limite + ": " + totalPrimos);
    executor.shutdown();
  }
}
