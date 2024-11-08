//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using System.Threading.Tasks;
//using System.Numerics;
//using IGE.Utils;
//public class InputManager : Entity
//{
//    private PlayerInput playerInput;
//    private PlayerInput.OnFootActions onFoot;

//    private PlayerMotor motor;
//    private PlayerLook look;
//    // Start is called before the first frame update
//    void Awake()
//    {
//        playerInput = new PlayerInput();
//        onFoot = playerInput.OnFoot;

//        motor = GetComponent<PlayerMotor>();
//        look = GetComponent<PlayerLook>();  

//        // anytime onfoot.jump is performed we are using a callback context (ctx) to call our jump function
//        onFoot.Jump.performed += ctx => motor.Jump();
//    }

//    // Update is called once per frame
//    void FixedUpdate()
//    {
//        //tell the playermotor to move using hte value from our movement action
//        motor.ProcessMove(onFoot.Movement.ReadValue<Vector2>());
//    }

//    private void Update()
//    {
//        look.ProcessLook(onFoot.Look.ReadValue<Vector2>());
//    }
//    //private void LateUpdate()
//    //{
//    //    look.ProcessLook(onFoot.Look.ReadValue<Vector2>());
//    //}

//    private void OnEnable()
//    {
//        onFoot.Enable();
//    }

//    private void OnDisable()
//    {
//        onFoot.Disable();
//    }
//}
