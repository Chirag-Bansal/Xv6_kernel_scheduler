#pragma once

//
// INVARIANT: w_deleted_count <= w_deleting_count <= w_cached_read_count <= shared_read_count <= r_reading_count <= r_cached_write_count <= shared_write_count <= w_writing_count <= w_deleted_count + MAX_SIZE
//
// INVARIANT:      w_writing_count      - w_deleted_count     <= MAX_SIZE
// =========>      w_writing_count      - w_cached_read_count <= MAX_SIZE
// =========>      shared_write_count   - w_cached_read_count <= MAX_SIZE
// =========>      shared_write_count   - shared_read_count   <= MAX_SIZE
//
//
// INVARIANT: 0 <= r_cached_write_count - r_reading_count
// =========> 0 <= r_cached_write_count - shared_read_count
// =========> 0 <= shared_write_count   - shared_read_count
//
//
// THEOREM: =========> 0 <= shared_write_count   - shared_read_count   <= MAX_SIZE
//





//
//
// Channel/Queue:
//
// Shared between Producer and Consumer
//
struct channel_t{
  public:

    //insert your code here
    int w_head;
    int r_head;
    int d_head;

    int w_round;
    int r_round;
    int d_round;

  public:

    //
    // Intialize
    //
    channel_t(){

      // insert your code here
      w_head = -1;
      r_head = -1;
      d_head = -1;

      w_round = -1;
      r_round = -1;
      d_round = -1;

    }
};



//
// Producer's (not shared)
//
struct writeport_t{
public:
    //insert your code here
    channel_t write_channel;
    size_t buffer_size;
public:

  //
  // Intialize
  //
  writeport_t(size_t tsize)
  {
    //insert code here
    buffer_size = tsize;

  }

public:

  //
  // no of entries available to write
  //
  // helper function for write_canreserve
  //
  size_t write_reservesize(){

    // insert your code here
    if(write_channel.w_head == -1){
      return buffer_size;
    }
    else if(write_channel.w_head > write_channel.d_head){
      return (buffer_size-1-write_channel.w_head+write_channel.d_head+1);
    }
    else if(write_channel.w_head == write_channel.d_head){
      if(write_channel.w_round == write_channel.d_round)
        return buffer_size;
      else
        return 0;
    }
    else{
      return (write_channel.d_head - write_channel.w_head);
    }
  }

  //
  // Can write 'n' entries?
  //
  bool write_canreserve(size_t n){

    // insert your code here
    if(write_reservesize()>=n){
      return true;
    }
    return false;
  }

  //
  // Reserve 'n' entries for write
  //
  size_t write_reserve(size_t n){
    // insert your code here
    if(n==1 && write_canreserve(1)){
      write_channel.w_head = (write_channel.w_head+1)%buffer_size;
      if(write_channel.w_head == 0)
        write_channel.w_round += 1;
      return write_channel.w_head;
    }

    return 0;
  }

  //
  // Commit
  //
  // Read/Write shared memory data structure
  //
  void write_release(channel_t& ch){

    // insert your code here
    write_channel.r_head = ch.r_head;  //to incorporate any changes that occured
    write_channel.r_round = ch.r_round;
    ch = write_channel;
  }


public:

  //
  //
  // Read/Write shared memory data structure
  //
  void read_acquire(channel_t& ch){

    //insert your code here
    write_channel.r_head = ch.r_head;
    write_channel.r_round = ch.r_round;
  }




  //
  // No of entires available to delete
  //
  size_t delete_reservesize(){
    //insert your code here
    if(write_channel.r_head==-1){
      return 0;
    }
    else if(write_channel.r_head>write_channel.d_head){
      return (write_channel.r_head - write_channel.d_head);
    }
    else if(write_channel.r_head==write_channel.d_head){
      if(write_channel.r_round == write_channel.d_round)
        return 0;
      else
        return buffer_size; 
    }
    else{
      (buffer_size-1-write_channel.d_head+write_channel.r_head+1);
    }
    // return 0;
  }

  //
  // Can delete 'n' entires?
  //
  bool delete_canreserve(size_t n){
    //insert your code here
    if(delete_reservesize()>=n){
      return true;
    }
    return false;
  }

  //
  // Reserve 'n' entires for deletion
  //
  size_t delete_reserve(size_t n){
    //insert your code here
    if(n==1 && delete_canreserve(n)){
      write_channel.d_head = (write_channel.d_head+1)%buffer_size;
      if(write_channel.d_head == 0)
        write_channel.d_round += 1;
      return write_channel.d_head;
    }
    return -1;
  }


  //
  // Update the state, if any.
  //
  void delete_release(){
    //insert your code here
  }


};


//
// Consumer's (not shared)
//
//
struct readport_t{
public:

  //insert your code here
  channel_t read_channel;
  size_t buffer_size;


public:
  //
  // Initialize
  //
  readport_t(size_t tsize)
  {

    //insert your code here
    buffer_size = tsize;

  }
  public:

  //
  // Read/Write shared memory data structure
  //
  void write_acquire(channel_t& ch){

    //insert your code here
    read_channel.w_head = ch.w_head;
    read_channel.d_head = ch.d_head;

    read_channel.w_round = ch.w_round;
    read_channel.d_round = ch.d_round;
  }

  //
  // no of entries available to read
  //
  size_t read_reservesize(){

    //insert your code here
    if(read_channel.w_head == -1){
      return 0;
    }
    else if(read_channel.w_head > read_channel.r_head){
      return (read_channel.w_head - read_channel.r_head);
    }
    else if(read_channel.w_head == read_channel.r_head){
      if(read_channel.w_round > read_channel.r_round)
        return buffer_size;
      else
        return 0;
    }
    else{
      return (buffer_size - 1 - read_channel.r_head + read_channel.w_head +1);  
    }
    return 0;
  }

  //
  // Can Read 'n' entires?
  //
  bool read_canreserve(size_t n){

    //insert your code here
    if(read_reservesize()>=n){
      return true;
    }
    return false;
  }

  //
  // Reserve 'n' entires to be read
  //
  size_t read_reserve(size_t n){

    //insert your code here
    if(n==1 && read_canreserve(n)){
      read_channel.r_head = (read_channel.r_head+1)%buffer_size;
      if(read_channel.r_head == 0)
        read_channel.r_round += 1;
      return read_channel.r_head;
    }
    return 0;
  }

  //
  // Read/write shared memory data structure
  //
  void read_release(channel_t& ch){

    //insert your code here
    read_channel.w_head = ch.w_head;
    read_channel.d_head = ch.d_head;

    read_channel.w_round = ch.w_round;
    read_channel.d_round = ch.d_round;

    ch = read_channel;

  }

};


