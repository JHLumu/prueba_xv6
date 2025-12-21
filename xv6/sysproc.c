#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  /**
   * Declaración de las variables
   */
  int status;

  /**
   * Se obtiene el estado
   */
  if(argint(0, &status) < 0)
    return -1;
  /** Se llama a la función interna encargada de toda la lógica 
   * de exit con el argumento status obtenido.
   * Boletin 1 Ejercicio 3: Se realiza un desplazamiento a la izquierda
   * para que los 8 bits más significativos sean el código de estado devuelto
   * y los 8 menos significativos indiquen que se ha finalizado de manera normal.
   *  */

  exit(status << 8);
  return 0;  // No alcanzable.
}

int
sys_wait(void)
{
  /** 
   * Declaración de las variables
   */
  int *p;

  /**
   * Se recupera el puntero donde almacenar el estado
   */
  if(argptr(0, (void*)&p, sizeof(int)) < 0)
    return -1;

  
  /** Se llama a la función interna encargada de toda la lógica 
   * de wait con el puntero obtenido. */
  return wait(p);
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  /** Declaración de variables */
  int addr;
  int n;

  /** Se obtiene el argumento a traves de la pila de usuario. En caso de error la 
   * función finaliza informando del error.
   */
  if(argint(0, &n) < 0)
    return -1;

  /**
   * Se guarda el tamaño actual de memoria del proceso.
   */
  struct proc *curproc = myproc();
  addr = curproc->sz;

  
  if(n > 0) {
    /** Si el proceso ha solicitado aumentar la memoria, se incrementa su tamaño pero no
     * se reserva memoria fisica (no se llama a growproc())
     */
    curproc->sz += n;
  } else {
    /** Boletin 2 Ejercicio 2
     * Si n es negativo, se decrementa la memoria. Para ello, se puede utilizar la función
     * growproc(n). En caso de error, finaliza la función informando del error.
     */
    if(growproc(n) < 0)
      return -1;
  }
  /** Se devuelve el tamaño antiguo de memoria */
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_date(void)
{
  /**
   * Declaración de variables.
   */
  struct rtcdate *r;

  /**
   * Se recupera el argumento de la pila del usuario. En caso de error, la función
   * finaliza informando del error con -1.
   */
  if(argptr(0, (void*)&r, sizeof(*r)) < 0)
    return -1;

  /** Lee la hora física y lo guarda en el puntero a la estructura rtcdate obtenido desde
   * la pila del usuario.
   */
  cmostime(r);
  
  return 0;
}
